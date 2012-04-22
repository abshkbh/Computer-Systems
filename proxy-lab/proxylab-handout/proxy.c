#include <stdio.h>

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *accept = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
static const char *connection = "Connection: close\r\n";
static const char *proxy_connection = "Proxy-Coneection: close\r\n";
static const char *get = "GET";
static const char *http = "HTTP/1.0";
static const char *host = "Host:";


void forward_request(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);



int main()
{

	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientaddr;

	/* Check command line args */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);

	listenfd = Open_listenfd(port);
	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		forward_request(connfd);
		Close(connfd);
	}


	return 0;
}

/*
 * forward_request - Forwards all GET requests to the intended
 *                   server.
 */
/* $begin doit */
void forward_request(int fd) 
{
    int is_correct;
    int clientfd;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], request[MAXLINE];
    char hostname[MAXLINE];
    rio_t rio;
  
    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) { 
       clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
        return;
    }
    read_requesthdrs(&rio);

    /* Parse URI from GET request */
    is_correct = parse_uri(uri,filename,hostname);
    if (is_correct < 0) {
	    return;
    }

   /* Once request is parsed we forward 
    * it to the parsed hostname */ 
     if ((clientfd = open_clientfd(hostname, HTTP_PORT)) < 0 ) {
         return; 
      }
     Rio_readinitb(&rio,clientfd);
     sprintf(request,"%s %s %s\r\n",get,filename,http);
     Rio_writen(clientfd,request,strlen(request));
     sprintf(request,"%s %s\r\n",host,hostname);
     Rio_writen(clientfd,request,strlen(request));
     Rio_writen(clientfd,user-agent,strlen(user-agent));
     Rio_writen(clientfd,accept,strlen(accept));
     Rio_writen(clientfd,accept-encoding,strlen(accept-encoding));
     Rio_writen(clientfd,connection,strlen(connection));
     Rio_writen(clientfd,proxy_connection,strlen(proxy_connection));

    /* Receive response from server and forward it
     * back to client. Once done close 'clientfd'  */

     /* <ANSHUMAN IMPLEMENT THIS>*/



}
/*
 * parse_uri - parse URI to retrieve hostname
 *
 * Example : http://www.google.com/index.html should return
 *           /index.html in filename
 *           www.google.com in hostname.
 *
 * Returns 1 if format OK , -1 if inscrutable format
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *filename, char *hostname) 
{
	char *start_ptr;
	char *end_ptr;
	int name_len;

	/* Looks for first '/' from starting of uri */
	if ((start_ptr = strchr(uri,'/')) == NULL) {
		return -1;
	}

	/* Move ptr by 2 bytes to point to starting of
	 * hostname */
	start_ptr = start_ptr + 2;

	/* This snippet looks for end of host name.
	 * Since, hostname can be followd by a :port.
	 * we need to first search for that. If unavailable
	 * we need to search for the last '/' in hostname
	 * and point end_ptr to one byte before it */
	if ((end_ptr = strchr(start_ptr,':')) == NULL) {

		if ((end_ptr = strchr(start_ptr,'/')) == NULL) {
			return -1;
		}

	}
	end_ptr --;

	/* Copy final name into hostname */
	name_len = end_ptr - start_ptr;
	strncpy(hostname,start_ptr,name_len);
	hostname[name_len] = '\0';

	/* Copy file name into filename */
	end_ptr ++;
	strcpy(filename,end_ptr);

	return 1;

}


/* EOF */


