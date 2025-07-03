# FUNCTIONS

## Table des matières

- [SYSCALLS](#syscalls)
  - [FILE MANAGEMENT](#file-management)
  - [DIRECTORY MANAGEMENT](#directory-management)
  - [ERROR MANAGEMENT](#error-management)
- [SOCKETS & NETWORK](#sockets--network)
  - [SOCKET CREATION & CONFIGURATION](#socket-creation--configuration)
  - [COMMUNICATION](#communication)
  - [DNS/IP](#dnsip)
  - [ENDIAN CONVERSIONS](#endian-conversions)
- [MULTIPLEX I/O](#multiplex-io)
  - [POSIX CLASSIC](#posix-classic)
  - [LINUX SPECIFIC](#linux-specific)

---

## SYSCALLS

### FILE MANAGEMENT

- **`int stat(const char *pathname, struct stat *statbuf)`**
  Obtenir des infos sur un fichier ou répertoire (taille, perms, type, date, etc.)
  Retourne 0 si succès et -1 si échec + set errno
  cf. `struct stat`

- **`int fcntl(int fd, int cmd, ...arg(s))`**
  Fonction polyvalente, permet de gérer des options sur des fd (non bloquant, gérer des verrous, dupliquer, etc.)

---

### DIRECTORY MANAGEMENT

- **`DIR *opendir(const char *name)`**
  Ouvre un répertoire et retourne un pointeur qui peut être utilisé pour lire son contenu avec `readdir()`
  Retourne NULL + set errno si erreur

- **`struct dirent *readdir(DIR *dirp)`**
  Lit une entrée ouverte avec `opendir()`. Permet d’itérer sur toutes les entrées
  Retourne un pointeur vers `struct dirent`, ou NULL à la fin du dossier
  Si erreur, retourne NULL + set errno
  cf. `struct dirent`

- **`int closedir(DIR *dirp)`**
  Ferme un répertoire ouvert avec `opendir()`
  Retourne 0 si succès et -1 si échec + set errno

- **`int chdir(const char *path)`**
  Sert à changer le répertoire de travail courant du processus
  Retourne 0 si succès et -1 si échec + set errno

---

### ERROR MANAGEMENT

- **`#include <errno.h>`**
  Variable globale qui sera mise à jour avec un code d’erreur quand une fonction standard ou système échoue.
  Doit être réinitialisée à zéro avant chaque appel de fonction

- **`char *strerror(int errnum)`**
  Convertit la valeur de `errno` en chaîne de caractères

- **`const char *gai_strerror(int errcode)`**
  Convertit un code d’erreur de la fonction `getaddrinfo()` en chaîne de caractères

---

## SOCKETS & NETWORK

### SOCKET CREATION & CONFIGURATION

- **`int socket(int domain, int type, int protocol)`**
  Crée un descripteur de socket.
  - `domain` = famille de protocoles (`AF_INET`, `AF_INET6`, `AF_UNIX`, etc.)
  - `type` = type de communication (`SOCK_STREAM`, `SOCK_DGRAM`, `SOCK_RAW`)
  - `protocol` = souvent 0
  Retourne un fd si succès, -1 sinon + set errno

- **`int socketpair(int domain, int type, int protocol, int sv[2])`**
  Crée deux sockets connectées entre elles pour communication bidirectionnelle (ex. entre processus)
  Retourne 0 si succès, -1 sinon + set errno

- **`int setsockopt(int sockfd, int level, int option_name, const void *option_value, socklen_t option_len)`**
  Configure une option de socket
  - `level` = `SOL_SOCKET`, `IPPROTO_TCP`, etc.
  - `option_value` = pointeur vers la valeur
  - `option_len` = taille en octets
  Retourne 0 si succès, -1 sinon + set errno

- **`int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`**
  Associe une socket à une adresse locale (IP + port)
  Obligatoire côté serveur
  Retourne 0 si succès, -1 sinon + set errno

- **`int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`**
  Côté client : établit une connexion vers une IP:port
  Bloquant par défaut
  Retourne 0 si succès, -1 sinon + set errno

- **`int listen(int sockfd, int backlog)`**
  Met la socket en mode écoute (TCP uniquement)
  Retourne 0 si succès, -1 sinon + set errno

- **`int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`**
  Accepte une connexion TCP entrante et crée un nouveau socket
  Fonction bloquante par défaut
  Retourne un nouveau fd si succès, -1 sinon + set errno

- **`int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`**
  Récupère l’adresse IP + port locale d’un socket
  Retourne 0 si succès, -1 sinon + set errno

- **`struct protoent *getprotobyname(const char *name)`**
  Cherche les infos d’un protocole (`tcp`, `udp`)
  Retourne un pointeur vers `struct protoent` ou NULL si introuvable

---

### COMMUNICATION

- **`ssize_t send(int sockfd, const void *buf, size_t len, int flags)`**
  Envoie des données via une socket
  Retourne le nombre d’octets envoyés, ou -1 si échec + set errno

- **`ssize_t recv(int sockfd, void *buf, size_t len, int flags)`**
  Reçoit des données via une socket
  Retourne nb d’octets reçus, 0 si la connexion est fermée, -1 si erreur

---

### DNS/IP

- **`int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)`**
  Traduit un nom ou IP + port en structures `sockaddr`
  Retourne 0 si succès, sinon code d’erreur à traduire avec `gai_strerror()`

- **`void freeaddrinfo(struct addrinfo *res)`**
  Libère la mémoire allouée par `getaddrinfo()`

---

### ENDIAN CONVERSIONS

- **`uint16_t htons(uint16_t hostshort)`**
  Convertit un entier 16 bits machine → réseau (big endian)

- **`uint32_t htonl(uint32_t hostlong)`**
  Idem que `htons` mais pour 32 bits

- **`uint16_t ntohs(uint16_t netshort)`**
  Convertit un entier 16 bits réseau → machine

- **`uint32_t ntohl(uint32_t netlong)`**
  Idem que `ntohs` mais pour 32 bits

---

## MULTIPLEX I/O

### POSIX CLASSIC

- **`int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)`**
  Surveille plusieurs fds (lecture, écriture, erreurs)
  Retourne nb de fds prêts, 0 si timeout, -1 si erreur

- **`int poll(struct pollfd fds[], nfds_t nfds, int timeout)`**
  Version moderne de `select`, plus flexible
  Retourne nb de fds prêts, 0 si timeout, -1 si erreur

---

### LINUX SPECIFIC

- **`int epoll_create(int size)`**
  Crée une instance `epoll` (paramètre `size` ignoré → mettre 0)
  Retourne un fd ou -1 si erreur

- **`int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`**
  Ajoute / modifie / supprime un fd surveillé dans un `epoll`
  epfd = descripteur de l’instance epoll
  op = type d’opération (EPOLL_CTL_ADD, EPOLL_CTL_MOD ou EPOLL_CTL_DEL)
  fd = descripteur de fichier à gérer
  event = pointeur vers struct epoll_event (sauf pour DEL où il peut être NULL)
  Retourne 0 si succès, -1 sinon + set errno

- **`int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`**
  Attend qu’au moins un fd soit prêt
  epfd = descripteur de l’instance epoll
  events = tableau de epoll_event alloué par l’utilisateur et qui sera rempli
  maxevents = taille du tableau events[]
  timeout = temps d’attente en millisecondes (0 = bloque pas, -1 = attend indéfiniment, >0 = attend ce nb de ms)
  Retourne le nb d’événements prêts, 0 si timeout, -1 si erreur

---
