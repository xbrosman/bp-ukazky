/*
    bp_komunikacia/uring_example/uring_example.c
    Author: Shuveb Hussain
    Source: https://unixism.net/loti/tutorial/cp_liburing.html
*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>

#define QD 2
#define BS (16 * 1024)

static int infd, outfd;

struct io_data
{
    int read;
    off_t first_offset, offset;
    size_t first_len;
    struct iovec iov;
};

// inicializacia uring
static int setup_context(unsigned entries, struct io_uring *ring)
{
    int ret;

    ret = io_uring_queue_init(entries, ring, 0);
    if (ret < 0)
    {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }

    return 0;
}

// funkcia zistuje velkost suboru
static int get_file_size(int fd, off_t *size)
{
    struct stat st;

    if (fstat(fd, &st) < 0)
        return -1;
    if (S_ISREG(st.st_mode))
    {
        *size = st.st_size;
        return 0;
    }
    else if (S_ISBLK(st.st_mode))
    {
        unsigned long long bytes;

        if (ioctl(fd, BLKGETSIZE64, &bytes) != 0) // Systemove volanie vycitava velkost blokoveho zariadenia a uklada vysledok do &bytes
            return -1;

        *size = bytes; // Ulozi vysledok do parametra funkcie
        return 0;
    }
    return -1;
}

static void queue_prepped(struct io_uring *ring, struct io_data *data)
{
    struct io_uring_sqe *sqe;

    sqe = io_uring_get_sqe(ring); // prebratie frontu na odoslanie
    assert(sqe);

    // vyber operacie
    if (data->read)
        io_uring_prep_readv(sqe, infd, &data->iov, 1, data->offset); // Ulozenie operacie pre citanie suboru read
    else
        io_uring_prep_writev(sqe, outfd, &data->iov, 1, data->offset); // Ulozenie operacie pre zapis write

    io_uring_sqe_set_data(sqe, data); //
}

static int queue_read(struct io_uring *ring, off_t size, off_t offset)
{
    struct io_uring_sqe *sqe;
    struct io_data *data;

    data = malloc(size + sizeof(*data)); // Alokacia dat na odoslanie do uring
    if (!data)
        return 1;

    sqe = io_uring_get_sqe(ring);
    if (!sqe)
    {
        free(data);
        return 1;
    }

    // nastavenie parametrov pre funkciu read
    data->read = 1;
    data->offset = data->first_offset = offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = size;
    data->first_len = size;

    io_uring_prep_readv(sqe, infd, &data->iov, 1, offset); //  Priprava structury pre asynchronne citanie nad infd suborovim deskriptorom
    io_uring_sqe_set_data(sqe, data);                      // Ulozenie operacie do frontu na odoslanie
    return 0;
}

static void queue_write(struct io_uring *ring, struct io_data *data)
{
    // nastavenie parametrov pre funkciu write
    data->read = 0;
    data->offset = data->first_offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = data->first_len;

    queue_prepped(ring, data);
    io_uring_submit(ring); // odoslanie požiadaviek do jadra vo fronte na odoslanie
}

int copy_file(struct io_uring *ring, off_t insize)
{
    unsigned long reads, writes;
    struct io_uring_cqe *cqe;
    off_t write_left, offset;
    int ret;

    write_left = insize;
    writes = reads = offset = 0;

    while (insize || write_left)
    {
        int had_reads, got_comp;

        // Podla velkosti suboru pripravuje citacie pozadavky
        had_reads = reads;
        while (insize)
        {
            off_t this_size = insize;

            if (reads + writes >= QD)
                break;
            if (this_size > BS)
                this_size = BS;
            else if (!this_size)
                break;

            if (queue_read(ring, this_size, offset))
                break;

            insize -= this_size;
            offset += this_size;
            reads++;
        }

        if (had_reads != reads)
        {
            ret = io_uring_submit(ring); // Odoslanie poziadaviek do jadra
            if (ret < 0)
            {
                fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
                break;
            }
        }

        // Vycitavanie z frontu na dokoncenie
        got_comp = 0;
        while (write_left)
        {
            struct io_data *data;

            if (!got_comp)
            {
                ret = io_uring_wait_cqe(ring, &cqe); // caka a ak najde vrati vysledny stav operacie
                got_comp = 1;
            }
            else
            {
                ret = io_uring_peek_cqe(ring, &cqe); // Vyhlada operaciu vo fronta na dokoncenie bez jej odstanenia
                if (ret == -EAGAIN)
                {
                    cqe = NULL;
                    ret = 0;
                }
            }
            if (ret < 0)
            {
                fprintf(stderr, "io_uring_peek_cqe: %s\n",
                        strerror(-ret));
                return 1;
            }
            if (!cqe)
                break;

            data = io_uring_cqe_get_data(cqe);  // vycitanie dat z io_uring_cqe, data priradené pomocou io_uring_sqe_set_data
            if (cqe->res < 0)
            {
                // osetrenie chyboveho stavu operacie
                if (cqe->res == -EAGAIN)
                {
                    // vykona sa ak res obsahuje hodnotu znaciacu "vykonat operaciu znovu"
                    queue_prepped(ring, data);
                    io_uring_cqe_seen(ring, cqe); 
                    continue;
                }
                fprintf(stderr, "cqe failed: %s\n",
                        strerror(-cqe->res));
                return 1;
            }
            else if (cqe->res != data->iov.iov_len)
            {
                // vykona sa ak sa precitalo menej byjtov ako sa cakalo a zaradi naspat do frontu aby sa dokoncila
                data->iov.iov_base += cqe->res; // odstani bajty zo zaciatku
                data->iov.iov_len -= cqe->res;  // odcita ich pocet
                queue_prepped(ring, data); // prida modifikovane data do frontu
                io_uring_cqe_seen(ring, cqe); // funkcia odstráni poziadavku z frontu 
                continue;
            }

            /*
             * All done. If write, nothing else to do. If read,
             * queue up corresponding write.
             * */

            // ak sa dokoncila operacia read data sa pripravia a nasledne ulozia do operacie write aby sa zapisali (copy) 
            if (data->read)
            {
                queue_write(ring, data);
                write_left -= data->first_len;
                reads--;
                writes++;
            }
            else
            {
                free(data);
                writes--;
            }
            io_uring_cqe_seen(ring, cqe); // funkcia odstráni dokoncenu poziadavku z frontu 
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct io_uring ring;
    off_t insize;
    int ret;

    if (argc < 3)
    {
        printf("Usage: %s <infile> <outfile>\n", argv[0]);
        return 1;
    }

    // otvorenie suboru s datami na kopirovanie
    infd = open(argv[1], O_RDONLY);
    if (infd < 0)
    {
        perror("open infile");
        return 1;
    }

    // otvorenie suboru kam sa data ulozia
    outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0)
    {
        perror("open outfile");
        return 1;
    }

    if (setup_context(QD, &ring))
        return 1;

    if (get_file_size(infd, &insize))
        return 1;

    ret = copy_file(&ring, insize);

    close(infd);
    close(outfd);
    io_uring_queue_exit(&ring); // uvolnenie struktury z pamate
    return ret;
}