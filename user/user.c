int mutex(void) {
    return syscall(SYS_mutex);
}

int mutex_lock(int fd) {
    return syscall(SYS_mutex_lock, fd);
}

int mutex_unlock(int fd) {
    return syscall(SYS_mutex_unlock, fd);
}
