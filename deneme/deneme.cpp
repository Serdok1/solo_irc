/*
** selectserver.c -- keyifli bir cok kullanicili chat sunucu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9034   // dinledigimiz port

int main(void)
{
    fd_set master;    // na dosya tanimlayici listesi
    fd_set read_fds;  // select() icin gecici dosya tanimlayici listesi
    struct sockaddr_in myaddr;     // sunucu adresi
    struct sockaddr_in remoteaddr; // istemci adresi
    int fdmax;        // azami dosya tanimlayici numarası
    int listener;     // dinlenen soket tanımlayıcı
    int newfd;        // yeni accept()lenecek soket tanımlayıcı
    char buf[256];    // istemci verisi için tampon
    int nbytes;
    int yes=1;        // setsockopt() SO_REUSEADDR için, aşağıda
    socklen_t addrlen;
    int i, j;

    FD_ZERO(&master);    // ana listeyi ve gecici listeyi temizle
    FD_ZERO(&read_fds);

    // dinleyiciyi yarat
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // "adres zaten kullanımda" mesajından kurtul
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                                                        sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // bind
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(PORT);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    // dinleyici soketi ana listeye ekle
    FD_SET(listener, &master);

    // en büyük dosya tanimlayicisi hatirla
    fdmax = listener; // so far, it's this one

    // ana döngü
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        // mevcut baglantilari tarayip okumaya hazir olanlari tespit et
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // bir tane yakaladik!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof(myaddr);
                    if ((newfd = accept(listener, (struct sockaddr *)&myaddr, &addrlen)) == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // ana listeye ekle
                        if (newfd > fdmax) {    // azami miktarı güncelle
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n", inet_ntoa(myaddr.sin_addr), newfd);
                    }
                } else {
                    // istemciden gelen veri icin gerekeni yap
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // bir hata var ya da istemci baglantiyi kesti
                        if (nbytes == 0) {
                            // baglanti kesilmis
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // ana listeden cikar
                    } else {
                        // istemciden bir miktar veri geldi
                        for(j = 0; j <= fdmax; j++) {
                            // gelen veriyi herkese yolla!
                            if (FD_ISSET(j, &master)) {
                                // dinleyici ve kendimiz haric
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // O KADAR CIRKIN ki!
            }
        }
    }

    return 0;
} 