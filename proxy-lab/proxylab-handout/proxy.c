#include <stdio.h>
#include "csapp.h"
#include <string.h>

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif


static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *accept_string = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
static const char *connection = "Connection: close\r\n";
static const char *proxy_connection = "Proxy-Coneection: close\r\n";
static const char *get = "GET";
static const char *http = "HTTP/1.0";
static const char *host = "Host:";


void read_requesthdrs(rio_t *rp);
void forward_request(int fd);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, 
		char *shortmsg, char *longmsg);



int main(int argc,char **argv)
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
		dbg_printf("Boy thats a connection!\n");
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, (SL *)&clientlen);
		dbg_printf("Accepted it!\n");
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

	printf("bing\n");

	Rio_readinitb(&rio,clientfd);
	dbg_printf("This is the URI: %s\n", uri);
	sprintf(request,"%s %s %s\r\n",get, uri,http);
	dbg_printf("This is the request: %s\n", request);
	Rio_writen(clientfd,(void *)request,strlen(request));
	sprintf(request,"%s %s\r\n",host,hostname);
	Rio_writen(clientfd,(void *)request,strlen(request));
	Rio_writen(clientfd,(void *)user_agent,strlen(user_agent));
	Rio_writen(clientfd,(void *)accept_string,strlen(accept_string));
	Rio_writen(clientfd,(void *)accept_encoding,strlen(accept_encoding));
	Rio_writen(clientfd,(void *)connection,strlen(connection));
	Rio_writen(clientfd,(void *)proxy_connection,strlen(proxy_connection));
	Rio_writen(clientfd,"\r\n\r\n",4);

	/* Receive response from server and forward it
	 * back to client. Once done close 'clientfd'  */

	/* <ANSHUMAN IMPLEMENT THIS>*/

	size_t n2;
	char buf2[MAXLINE];
	rio_t rio2;

	dbg_printf("Server response starts here:\n");
	Rio_readinitb(&rio2, clientfd);
	while((n2 = Rio_readlineb(&rio2, buf2, MAXLINE)) != 0 ) {
		dbg_printf("%s\n", buf2);
		Rio_writen(fd, buf2, n2);
	}
	Close(clientfd);
	dbg_printf("It is closing the conn!\n");
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
	int flag = 1;

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

		/* flag = 0 is used to handle clause when
		 * default file is given e.g. http://www.google.com/
		 * we want file as "/" */
		if ((end_ptr = strchr(start_ptr,'/')) == NULL) {
			flag = 0;
			strcpy(filename,end_ptr);
		}

	}
	end_ptr --;

	/* Copy final name into hostname */
	name_len = (end_ptr - start_ptr) + 1;
	strncpy(hostname,start_ptr,name_len);
	hostname[name_len] = '\0';

	/* Copy file name into filename */
	if (flag) {
		end_ptr ++;
		strcpy(filename,end_ptr);
	}
	return 1;

}

/*
 * read_requesthdrs - read and parse HTTP request headers
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
	char buf[MAXLINE];

	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
	while(strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		char *shortmsg, char *longmsg) 
{
	char buf[MAXLINE], body[MAXBUF];

	/* Build the HTTP response body */
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

	/* Print the HTTP response */
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}





/* EOF */


