# FUNCTIONS

## SYSCALLS

### FILE MANAGEMENT

-**int stat(const char *pathname, struct stat *statbuf)**

    Obtenir des infos sur un fichier ou répertoire (taille, perms, type, date, etc.)
    retourne 0 si succès et -1 si échec + set errno
    cf. struct stat

-**int fcntl(int fd, int cmd, ...arg(s))**

    Fonction polyvalente, permet de gérer des options sur des fd. (non bloquant,
    gérer des verrous, dupliquer, etc.)

### DIRECTORY MANAGEMENT

-**DIR *opendir(const char *name)**

    Ouvrir un répertoire et retourne un pointeur qui peut être utilisé pour lire
    son contenu avec readdir();
    retourne NULL + set errno si erreur

-**struct dirent *readdir(DIR *dirp)**

    Lit une entrée ouverte avec opendir(). Permet d’itérer sur toutes les entrées.
    Retourne un pointeur vers struct dirent. Retourne NULL à la fin du dossier
    si erreur retourne NULL + set errno
    cf. struct dirent

-**int closedir(DIR *dirp)**

    Ferme un répertoire ouvert avec opendir();
    retourne 0 si succès et -1 si échec + set errno

-**int chdir(const char *path)**

    Sert à changer le répertoire de travail courant du processus
    retourne 0 si succès et -1 si échec + set errno

### ERROR MANAGEMENT

-**#include <errno.h>**

    Variable globale qui sera mise à jour avec un code d’erreur quand une fonction
    standard ou système échoue. Doit être réinitialisée à zéro avant chaque appel de fonction

-**char *strerror(int errnum)**

    Convertir la valeur de errno en chaîne de caractères

-**const char *gai_strerror(int errcode)**

    Convertir un code d’erreur de la fonction getaddrinfo() en chaîne de caractères

## SOCKETS & NETWORK

### SOCKET CREATION & CONFIGURATION

-**int socket(int domain, int type, int protocol)**

    Créer un descripteur de socket. Renvoie un fd qui peut être utilisé avec bind(),
    connect(), send(), recv(), etc.
    domain = famille de protocoles (AF_INET (ipv4), AF_INET6 (ipv6), AF_UNIX (socket local), etc.)
    type = type de communication (SOCK_STREAM (TCP), SOCK_DGRAM (UDP), SOCK_RAW (paquets bruts), etc.)
    protocol = souvent 0
    retourne un socket descriptor si succès et -1 si échec + set errno

-**int socketpair(int domain, int type, int protocol, int sv[2])**

    Crée deux sockets connectées entre elles pour communication bidirectionnelle
    entre deux processus ou threads sur la même machine.
    Alternative à pipe()
    domain = famille de protocoles (AF_INET, etc.)
    type = type de communication (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, etc.)
    protocol = souvent 0
    sv[2] = tableau des deux descripteurs de sockets à lier
    retourne 0 si succès et -1 si échec + set errno

-**int setsockopt(int sockfd, int level, int option_name, const void *option_value, socklen_t option_len)**

    Permet de configurer une option d’une socket afin de : modifier son comportement,
    réutiliser un port, activer/désactiver des fonctionnalités spécifiques
    sockfd = socket descriptor
    level = niveau de l’option (SOL_SOCKET, IPPROTO_TCP, etc.)
    option_name = nom de l’option à définir
    option_value = pointeur vers la valeur à attribuer
    option_len = taille en octets de option_value
    retourne 0 si succès et -1 si échec + set errno

-**int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)**

    Associer une socket à une adresse locale (IP + port)
    Obligatoire côté serveur pour indiquer sur quelle IP écouter
    sockfd = descripteur du socket obtenu avec socket()
    addr = ptr vers une struct d’adresse (sockaddr_in)
    addrlen = taille de la structure pointée (sizeof(struct sockaddr_in))
    retourne un socket descriptor si succès et -1 si échec + set errno

-**int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)**

    Permet à un socket côté client d’établir une connexion avec un serveur distant (IP + port)
    sockfd = descripteur du socket obtenu avec socket()
    addr = ptr vers une struct d’adresse (sockaddr_in)
    addrlen = taille de la structure pointée
    Bloquant par défaut, il attend jusqu’à ce que la connexion soit établie
    ou échoue
    retourne un socket descriptor si succès et -1 si échec + set errno

-**int listen(int sockfd, int backlog)**

    Met la socket créée avec socket() et liée avec bind() en mode passive. Prête
    à accepter les connexions via accept(). Fonctionne seulement avec des sockets TCP
    sockfd = descripteur de socket
    backlog = taille de la file d’attente des connexions en attente
    retourne 0 si succès et -1 si échec + set errno

-**int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)**

    Accepte une connexion TCP entrante sur un socket qui écoute avec listen()
    Crée un nouveau socket dédié à la communication avec ce client
    Fonction bloquante par défaut, elle attend qu’un client se connecte.
    sockfd = descripteur du socket obtenu avec socket()
    addr = ptr vers une struct d’adresse (sockaddr_in)
    addrlen = taille de la structure pointée
    retourne un socket descriptor si succès et -1 si échec + set errno

-**int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)**

    Permet de récupérer l’adresse locale (IP + port) à laquelle un socket est lié
    sockfd = descripteur du socket obtenu avec socket()
    addr = ptr vers une struct d’adresse (sockaddr_in)
    addrlen = taille de la structure pointée
    retourne 0 si succès et -1 si échec + set errno

-**struct protoent *getprotobyname(const char *name)**

    Recherche dans une base de données des protocoles réseaux l’entrée correspondante
    à ce nom et retourne une structure protoent qui contient les infos
    name = nom du protocole réseau (tcp ou udp)
    cf. struct protoent
    retourne NULL si protocole introuvable

### COMMUNICATION

-**ssize_t send(int sockfd, const void *buf, size_t len, int flags)**

    Envoie des données à travers une socket réseau
    sockfd = descripteur du socket obtenu avec socket()
    buf = pointeur vers les données à envoyer
    len = taille (octets) des données à envoyer
    flags = options supp (souvent 0)
    retourne le nb d’octets envoyés si succès et -1 si échec + set errno

-**ssize_t recv(int sockfd, void *buf, size_t len, int flags)**

    Reçoit des données venant d’un socket
    sockfd = descripteur du socket connecté obtenu après accept() ou connect()
    buf = buffer où stocker les données reçues
    len = taille max à lire (taille du buffer)
    flags = options supp (souvent 0)
    retourne le nb d’octets reçus ou 0 si connexion fermée ou -1 si échec + set errno

### DNS/IP

-**int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)**

    Traduit un nom d’hôte (ou adresse IP) et un service (port) en une ou plusieurs
    structures sockaddr utilisables avec socket(), connect(), bind()
    node = nom d’hôte ou IP ("localhost", "192.168.1.10")
    service = port ou nom de service ("80", "http")
    hints = structure optionnelle pour filtrer les résultats (famille, type de socket)
    res = pointeur de sortie qui recevra la liste des résultats (liste chaînée à libérer
    avec freeaddrinfo())
    retourne 0 si succès sinon code d’erreur à traduire avec gai_strerror()
    cf. struct addrinfo

-**void freeaddrinfo(struct addrinfo *res)**

    Libère la mémoire allouée par getaddrinfo()
    res = pointeur retourné par getaddrinfo()

### ENDIAN CONVERSIONS

-**uint16_t htons(uint16_t hostshort)**

    Convertir un entier 16 bits de l’ordre des octets de la machine (little endian)
    locale vers l’ordre des octets utilisé sur le réseau (big endian)
    retourne le même entier mais converti

-**uint32_t htonl(uint32_t hostlong)**

    Idem que pour htons mais pour un entier 32 bits

-**uint16_t ntohs(uint16_t netshort)**

    Idem que pour htons mais cette fois-ci conversion réseau → machine locale

-**uint32_t ntohl(uint32_t netlong)**

    Idem que pour htonl mais cette fois-ci conversion réseau → machine locale

## MULTIPLEX I/O

### POSIX CLASSIC

-**int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)**

    Permet de surveiller plusieurs fds (socket, pipes, stdin) pour savoir quand
    ils sont prêts à lire, écrire ou signaler une erreur, sans bloquer.
    nfds = max fd + 1 parmi tous ceux que tu surveilles
    readfds = sockets à surveiller pour la lecture (read-ready)
    writefds = sockets à surveiller pour l’écriture (write-ready)
    exceptfds = sockets à surveiller pour les erreurs (rare)
    timeout = délai d’attente maximum (ou NULL pour bloquer indéfiniment)
    retourne le nb de descripteurs prêts sinon 0 si rien n’est prêt ou -1 erreur + set errno

-**int poll(struct pollfd fds[], nfds_t nfds, int timeout)**

    Comme select mais en mieux (pas de limite de fd, tableau à la place de struct,
    meilleures perfs)
    fds[] = tableau de structures pollfd, un par fd à surveiller
    nfds = nombre d’éléments dans le tableau
    timeout = temps d’attente en millisecondes (0 = bloque pas, -1 = attend indéfiniment,
    >0 = attend ce nb de ms)
    retourne le nb de descripteurs prêts sinon 0 si rien n’est prêt ou -1 erreur + set errno

### LINUX SPECIFIC

-**int epoll_create(int size)**

    Crée un descripteur d’objet epoll
    size = nb max de fds à surveiller (plus d’actualité, doit être mis à 0)
    renvoie >=0 (descripteur d’objet epoll) ou -1 si échec + set errno

-**int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)**

    Permet de gérer la liste de descripteurs surveillés par une instance epoll
    Utilisé pour ajouter, modifier ou retirer un fd (socket, fd, etc.) dans l’instance
    créée avec epoll_create()
    epfd = descripteur de l’instance epoll
    op = type d’opération (EPOLL_CTL_ADD, EPOLL_CTL_MOD ou EPOLL_CTL_DEL)
    fd = descripteur de fichier à gérer
    event = pointeur vers struct epoll_event (sauf pour DEL où il peut être NULL)
    renvoie 0 si succès sinon -1 + set errno

-**int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)**

    Bloque (ou non) et attend qu’un ou plusieurs fds surveillés par epoll deviennent
    prêts
    epfd = descripteur de l’instance epoll
    events = tableau d’epoll_event alloué par l’utilisateur et qui sera rempli
    maxevents = taille du tableau events[]
    timeout = temps d’attente en millisecondes (0 = bloque pas, -1 = attend indéfiniment,
    >0 = attend ce nb de ms)
    renvoie >0 nb d’événements prêts sinon 0 timeout expiré sinon -1 en cas d’échec + set errno

