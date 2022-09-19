### INTRODUÇÃO
Pipes são uma ferramenta de IPC. A ideia é ter um arquivo em memória que bloqueia em leitura e escrita, até que ambas partes (leitor e escritor) estejam presentes no arquivo. Também existem FIFOs (named pipes) que utilizam um nome no sistema de arquivos para ser acessado de processos distintos, em quanto que pipes são restritos ao mesmo arquivo/diferentes threads/forks (processos da mesma hierarquia).

O pipe é manipulado pelo SO, oferecendo funções para interação ao usuário:

- duas syscalls
- funções especificadas numa struct

```c
SYSCALL_DEFINE1(pipe, int __user *, fildes)
{
	return do_pipe2(fildes, 0);
}

// (...)

SYSCALL_DEFINE2(pipe2, int __user *, fildes, int, flags)
{
	return do_pipe2(fildes, flags);
}

// (...)

const struct file_operations pipefifo_fops = {
	.open		= fifo_open,
	.llseek		= no_llseek,
	.read_iter	= pipe_read,
	.write_iter	= pipe_write,
	.poll		= pipe_poll,
	.unlocked_ioctl	= pipe_ioctl,
	.release	= pipe_release,
	.fasync		= pipe_fasync,
	.splice_write	= iter_file_splice_write,
};
```



Explicar pra que serve e dar um exemplo rápido do seu uso no terminal.

A estrutura do pipe é (e informações mantidas):

```c
/**
 *	struct pipe_inode_info - a linux kernel pipe
 *	@mutex: mutex protecting the whole thing
 *	@wait: reader/writer wait point in case of empty/full pipe
 *	@nrbufs: the number of non-empty pipe buffers in this pipe
 *	@buffers: total number of buffers (should be a power of 2)
 *	@curbuf: the current pipe buffer entry
 *	@tmp_page: cached released page
 *	@readers: number of current readers of this pipe
 *	@writers: number of current writers of this pipe
 *	@files: number of struct file referring this pipe (protected by ->i_lock)
 *	@waiting_writers: number of writers blocked waiting for room
 *	@r_counter: reader counter
 *	@w_counter: writer counter
 *	@fasync_readers: reader side fasync
 *	@fasync_writers: writer side fasync
 *	@bufs: the circular array of pipe buffers
 *	@user: the user who created this pipe
 **/
struct pipe_inode_info {
	struct mutex mutex;
	wait_queue_head_t wait;
	unsigned int nrbufs, curbuf, buffers;
	unsigned int readers;
	unsigned int writers;
	unsigned int files;
	unsigned int waiting_writers;
	unsigned int r_counter;
	unsigned int w_counter;
	struct page *tmp_page;
	struct fasync_struct *fasync_readers;
	struct fasync_struct *fasync_writers;
	struct pipe_buffer *bufs;
	struct user_struct *user;
};

// (...)

/**
 *	struct pipe_buffer - a linux kernel pipe buffer
 *	@page: the page containing the data for the pipe buffer
 *	@offset: offset of data inside the @page
 *	@len: length of data inside the @page
 *	@ops: operations associated with this buffer. See @pipe_buf_operations.
 *	@flags: pipe buffer flags. See above.
 *	@private: private data owned by the ops.
 **/
struct pipe_buffer {
	struct page *page;
	unsigned int offset, len;
	const struct pipe_buf_operations *ops;
	unsigned int flags;
	unsigned long private;
};
```

[Exemplo de utilização](https://github.com/LombardiDaniel/OS-Assignments/tree/main/Pipe+Internals)


### CRIAÇÃO
A criação de pipe é feita pela função `do_pipe2()`. Outras invocações como `pipe()` e `pipe2()` são meros wrappers em torno da estrutura, como visto nas syscalls anteriores.

Assim, a criação do pipe é feita em `do_pipe2()`:

```c
static int do_pipe2(int __user *fildes, int flags)
{
	struct file *files[2];
	int fd[2];
	int error;

	error = __do_pipe_flags(fd, files, flags);
	if (!error) {
		if (unlikely(copy_to_user(fildes, fd, sizeof(fd)))) {
			fput(files[0]);
			fput(files[1]);
			put_unused_fd(fd[0]);
			put_unused_fd(fd[1]);
			error = -EFAULT;
		} else {
			fd_install(fd[0], files[0]);
			fd_install(fd[1], files[1]);
		}
	}
	return error;
}
```

Para criação do pipe, precisa-se inicializar ambos ponteiros (ida e volta de informações) e copialos ao código do usuário. Isso é feito com a função `copy_to_user()`, que equivale à um `memcopy` de `kernel space` para `user space`. Essa função `do_pipe2()` é a chamda mais alto nível do processo de criação de pipes.

Passando para a função `__do_pipe_flags()`:

```c
static int __do_pipe_flags(int *fd, struct file **files, int flags)
{
	int error;
	int fdw, fdr;

	if (flags & ~(O_CLOEXEC | O_NONBLOCK | O_DIRECT | O_NOTIFICATION_PIPE))
		return -EINVAL;

	error = create_pipe_files(files, flags);
	if (error)
		return error;

	error = get_unused_fd_flags(flags);
	if (error < 0)
		goto err_read_pipe;
	fdr = error;

	error = get_unused_fd_flags(flags);
	if (error < 0)
		goto err_fdr;
	fdw = error;

	audit_fd_pair(fdr, fdw);
	fd[0] = fdr;
	fd[1] = fdw;
	return 0;

 err_fdr:
	put_unused_fd(fdr);
 err_read_pipe:
	fput(files[0]);
	fput(files[1]);
	return error;
}
```


A chamada de `__do_pipe_flags()` trata de filtrar as flags passadas pelo usuário. No caso de sucesso, a função `create_pipe_files()` é invocada.

```c
int create_pipe_files(struct file **res, int flags)
{
	struct inode *inode = get_pipe_inode();
	struct file *f;
	int error;

	if (!inode)
		return -ENFILE;

	if (flags & O_NOTIFICATION_PIPE) {
		error = watch_queue_init(inode->i_pipe);
		if (error) {
			free_pipe_info(inode->i_pipe);
			iput(inode);
			return error;
		}
	}

	f = alloc_file_pseudo(inode, pipe_mnt, "",
				O_WRONLY | (flags & (O_NONBLOCK | O_DIRECT)),
				&pipefifo_fops);
	if (IS_ERR(f)) {
		free_pipe_info(inode->i_pipe);
		iput(inode);
		return PTR_ERR(f);
	}

	f->private_data = inode->i_pipe;

	res[0] = alloc_file_clone(f, O_RDONLY | (flags & O_NONBLOCK),
				  &pipefifo_fops);
	if (IS_ERR(res[0])) {
		put_pipe_info(inode, inode->i_pipe);
		fput(f);
		return PTR_ERR(res[0]);
	}
	res[0]->private_data = inode->i_pipe;
	res[1] = f;
	stream_open(inode, res[0]);
	stream_open(inode, res[1]);
	return 0;
}
```

A função `create_pipe_files()` faz exatamente isso, cria os arquivos utilizados no pipe, desta vez, tratando as flags especificadas pelo usuário. Como toda entrada no VFS, são criados inodes. Porém para evitar que estes sejam futuramente escritos em disco (em suas posições de arquivos), o inode é inicialmente já é marcado como *dirty*, evitando que seja colocado na dirty list, já que isso 'enganará' `mark_inode_dirty()`, fazendo parecer que o inode do pipe já está na lista. Além disso, sendo representados por inodes, eles sempre poderão ser operados como arquivos, seguindo o paradigma de "In linux, everything is a file".
