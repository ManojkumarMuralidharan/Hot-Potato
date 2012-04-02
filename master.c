#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct child{
	//to recv
	int fd;

	//to send
	//
	int rd;

	int id;
	//int left_fd;
	//int right_fd;
	int listenport;
	char *ip;
	struct hostent *ihp;
	struct child *next;

};

static int id=1;
static int child_index=0;
main (int argc, void *argv[])
{

int socket_ser,server_port,rc,n_players,errno=1;
struct hostent* server_info,*ihp;
struct sockaddr_in server, incoming;
struct child *list=NULL;
int received_hops;


// check for inputs
if(argv[1]==NULL){;
printf("Please provide port info -  master <port-number> <number-of-players> <hops>\n");
exit(1);
}
if(argv[2]==NULL){
	printf("Please provide No. of Players info -  master <port-number> <number-of-players> <hops>\n");
exit(1);
}
if(argv[3]==NULL){
	printf("Please provide No. of Hops info -  master <port-number> <number-of-players> <hops>\n");
exit(1);
}

received_hops=atoi(argv[3]);

char host[64];

	socket_ser = socket(AF_INET, SOCK_STREAM, 0);
	if ( socket_ser < 0 ) {
    perror("socket:");
    exit(socket_ser);
  	}
	
	gethostname(host, sizeof host);
  	
  	server_info= gethostbyname(host); 
	
	server_port=atoi(argv[1]);

	n_players=atoi(argv[2]);

	if(n_players==1){
	printf("Provide more no. of players");
	exit(0);
	}

	if ( server_info == NULL ) {
    fprintf(stderr, "%s: host not found\n", argv[0]);
    exit(1);
  	}
	//printf("hello");
	

	server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
  	memcpy(&server.sin_addr, server_info->h_addr_list[0], server_info->h_length);
  	
  	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);
  /* connect to socket at above addr and port */
	rc = bind(socket_ser,(struct sockaddr *)&server, sizeof(server));
	
	  if ( rc < 0 ) {
	    perror("bind:");
	    exit(rc);
	  }


	  if (listen(socket_ser, n_players) == -1) {
        perror("listen");
        exit(3);
      }

      int players;
      list=malloc(sizeof(struct child)*n_players);
     	  if ( list == NULL ) {
    		perror("malloc");
    		exit(errno);
  		}

  		printf("\nPlayers=%d\n",n_players);
  		printf("\nHops=%d\n",received_hops);
      for(players=1;players<=n_players;players++){

      	int p,len;
      	len=sizeof server;
      	struct sockaddr_in temp;
      	struct hostent* temp_host_info;
      	//incoming=NULL;
      	p=accept(socket_ser, (struct sockaddr *)&temp, &len);

		temp_host_info = gethostbyaddr((char *)&temp.sin_addr,sizeof(struct in_addr), AF_INET);

		char *ip=malloc(sizeof 64);
		ip = inet_ntoa(*((struct in_addr *)temp_host_info->h_addr_list[0]));

      	//printf("\ncome in=%d %s\n",p,temp_host_info->h_name);
      	
      	struct child *temp_list=list;

      	//if(temp_list==NULL){
		    		
		      		list[child_index].fd=p;
		      		//list[child_index].next=NULL;
		      		list[child_index].id=id++;
		        	list[child_index].ihp=temp_host_info;
		        	list[child_index].ip=malloc(sizeof 64);
		        	strcpy(list[child_index].ip,ip);
		        	//list[child_index].listenport
		printf("\nPlayer %d is on %s \n",id-1,temp_host_info->h_name);
		
		int recptn_len;
		char buf[35];
		recptn_len = recv(p, buf, 32, 0);
		buf[recptn_len] = '\0';
		//printf("\nrecived len = %d\n",recptn_len);
		//printf("\nrecived port = %s\n",buf);
		char *rec_port=malloc(1);
		strcpy(rec_port,buf);
		//printf("\nrecived port = %s\n",rec_port);
		//printf("-%d-\n",atoi(rec_port));
		list[child_index].listenport=atoi(rec_port);
			
      //	}
      //	else{
      			
		/*      	while(temp_list->next!=NULL){
		//      		temp_list=temp_list->next;
		//      		}
		      		temp_list->next=malloc(sizeof(struct child));
		      		temp_list->next->fd=p;
		      		temp_list->next->next=NULL;
		      		temp_list->id=id++;
		      		temp_list->ihp=temp_host_info;
		      	
      	}*/

      	

      	// add the fd to an array of descriptors

      	child_index++;
      }

  

      //struct child *temp_list=list;
      int count=0;
      if(child_index<=1){
      	printf("\ntoo litle childrens\n");
      }
      else{
	      while(count<child_index){
	      	  
	      	  //Create a temp socket
	      	  /*int temp_socket,temp_connection;
	      	  struct hostent *temp_info;
	      	  struct sockaddr_in temp_socket_info;
	      	  temp_socket = socket(AF_INET, SOCK_STREAM, 0);
				if ( temp_socket < 0 ) {
    			perror("socket:");
    			exit(temp_socket);
  				}
  			  temp_info=gethostbyname(list[count].ihp->h_name);

  			  	temp_socket_info.sin_family = AF_INET;
   				temp_socket_info.sin_port = htons(list[count].listenport);
  				memcpy(&temp_socket_info.sin_addr, temp_info->h_addr_list[0], temp_info->h_length);

  			  //server.sin_port = htons(server_port);
  			  int temp_len;
  			  temp_len=sizeof temp_socket_info;
  			  temp_connection = connect(temp_socket, (struct sockaddr *)&temp_socket_info, sizeof(temp_socket_info));
			  if ( temp_connection < 0 ) {
			    perror("connect:");
			    exit(temp_connection);
			  }*/

  			   int temp_send_len;
  			   char *convert_port=malloc(1);
  			   sprintf(convert_port,"%d:%d:%d",list[(count+1)%(child_index)].listenport,list[count].id,n_players);
			   char *str=malloc(strlen(list[(count+1)%(child_index)].ip)+strlen(convert_port)+2 );
			   strcpy(str,list[(count+1)%(child_index)].ip);
			   strcat(str,":");
			   strcat(str,convert_port);
			   //strcat(str,":");
			   //strcat(str,list[count].id);
			   //sprintf(str,"%d",peerport);
			   //printf("\nchildname sent-here=%s\n",str);



				temp_send_len = send(list[count].fd,str, strlen(str), 0);
			    if ( temp_send_len != strlen(str) ) {
			      perror("send");
			      exit(1);
			    }

			    free(str);
			   str=malloc(1);
			   //strcpy(str,list[(count+1)%(child_index)].listenport);
			   sprintf(str,"%d",list[(count+1)%(child_index)].listenport);
			   //printf("\nchildport sent-here=%s\n",str);
			   /*
			    temp_send_len = send(list[count].fd, str, strlen(str), 0);
			    if ( temp_send_len != strlen(str) ) {
			      perror("send");
			      exit(1);
			    }*/

	      	 // printf("\nchild is in %s:%d\n",list[count].ihp->h_name,list[count].listenport);
	      	  count++;

	      }
  	  }
  	  
  	  int temp_send_len;
  	  char *str="hello";
  	  temp_send_len = send(list[0].fd,str, strlen(str), 0);
			    if ( temp_send_len != strlen(str) ) {
			      perror("send");
			      exit(1);
			    }else{
			    	//printf("\n sent \n");
			    }

	char buf[35];
	int recptn_len;
	recptn_len = recv(list[0].fd, buf, 32, 0);
	buf[recptn_len] = '\0';
	//temp_send_len = recv(list[0].fd,str, strlen(str), 0);
	//printf("\nhi%d\n",recptn_len);
			    if ( recptn_len != strlen(buf) ) {
			      perror("recv");
			      exit(1);
			    }else{
			    	//printf("\n %s \n",buf);
			    }
	
	
	char *hot_potato=malloc(strlen(argv[3])+2);
	  if ( hot_potato == NULL ) {
   		 perror("malloc");
   		 exit(errno);
 	 }
	int msg_len=0;
	int msg_size=strlen(argv[3])+2;
	int hops=msg_size;
	while(hops>0) {
        msg_len++;
      hops=hops/10;
    }
	char *sizeofmsg=malloc(msg_len+1);
	sprintf(sizeofmsg,"%d",msg_size);
	sizeofmsg[msg_len+1]='\0';

	/*temp_send_len = send(list[0].fd,sizeofmsg, strlen(sizeofmsg), 0);
			    if ( temp_send_len != strlen(sizeofmsg) ) {
			      perror("send");
			      exit(1);
			    }else{
			    	printf("\n sent %s\n",sizeofmsg);
			    }
*/
	if(received_hops==0){
	printf("No of hops %d, Game shutting down",received_hops);
	int temp_send;
	temp_send = send(list[get_random(n_players)].fd,"endgame", 7, 0);
			    if ( temp_send != 7 ) {
			      perror("send");
			      exit(1);
			    }
	char *temp_buf=malloc(100);
	count=0;
	while(count<child_index){

		//if(count!=potato_received){
				temp_send_len = recv(list[count].fd,temp_buf, 100, 0);
			    if ( temp_send_len != strlen(temp_buf) ) {
			      perror("send");
			      exit(1);
			    }
			   // printf("\n%d\n",count);
		//	}
		count++;
	}	
	exit(0);
	
	}
	sprintf(hot_potato,"%d",received_hops);

	int selected_player=get_random(n_players);

	printf("\n All players present, sending potato to player %d\n",selected_player+1);


	temp_send_len = send(list[selected_player].fd,hot_potato, strlen(hot_potato), 0);
			    if ( temp_send_len != strlen(hot_potato) ) {
			      perror("send");
			      exit(1);
			    }else{
			    	//printf("\n sent %s\n",hot_potato);
			    }
	hot_potato[strlen(hot_potato)+1]='\0';

	int rv,nfds=0;
	char *tmp_buf;
	fd_set listenfds;
	
	FD_ZERO(&listenfds);

	count=0;
	while(count<child_index){
		FD_SET(list[count].fd,&listenfds);
		nfds=MAX(nfds,list[count].fd);
		count++;
	}
	nfds=nfds+1;
	
	char *close="endgame";
	int n,i_fd,break_flag=0,potato_received=0,child_ended=0;

	rv = select(nfds,&listenfds,NULL,NULL,0);
	
		if (rv == -1) {
		    perror("select"); // error occurred in select()
		} else if (rv == 0) {
		    printf("Timeout occurred!  No data after 10.5 seconds.\n");
		} else {
			//receive potato and print it
			for(i_fd=0;i_fd<=nfds;i_fd++){
					    if(FD_ISSET(i_fd,&listenfds)!=0){
					for(count=0;count<child_index;count++){
						if(i_fd==list[count].fd){

					if(potato_received==0){
					int read_flag=0;
					do{
					sleep(1);
					if(ioctl(list[count].fd, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
		 						 perror("ioctl");
		  						 exit(1);
								}
					if(n>0){
					
						if(read_flag==0){
							if(n>1024)
							n=1024;
						tmp_buf=malloc(n+1);
						recptn_len = recv(list[count].fd, tmp_buf, n, 0);
						//printf("\nn=%d-recp-len-%d\n",n,recptn_len);
						tmp_buf[recptn_len] = '\0';
				//temp_send_len = recv(list[0].fd,str, strlen(str), 0);
						//printf("\nhi%d\n",recptn_len);
						    if ( recptn_len != strlen(tmp_buf) ) {
						      perror("recv");
						      exit(1);
						    }else{
						    	//printf("\n %s \n",tmp_buf);
						    	break_flag=1;
						    }
						    read_flag=1;
							}else if(read_flag==1){
							char *temp_sizemsg;
							if(n>1024)
							n=1024;
						   	temp_sizemsg=malloc(strlen(tmp_buf)+n+1);
						   	strcpy(temp_sizemsg,tmp_buf);
						   	//free(sizemsg);
						   	tmp_buf=malloc(n+1);
							if ( tmp_buf == NULL ) {
	    						perror("malloc");
	    						exit(-1);
	  						}
							//memset(sizemsg,'\0',sizeof(sizemsg)-1);
					  		recptn_len = recv(list[count].fd, tmp_buf, n, 0);
					      	if ( recptn_len < 0 ) {
								perror("recv");
								exit(1);
						   	}
						   	tmp_buf[recptn_len]='\0';
						   	strcat(temp_sizemsg,tmp_buf);
						   	tmp_buf=malloc(strlen(temp_sizemsg)+1);
						   	strcpy(tmp_buf,temp_sizemsg);

								
							}
					  }
					}while(n>0);




				temp_send_len = send(list[count].fd,close, strlen(close), 0);
			    if ( temp_send_len != strlen(close) ) {
			      perror("send");
			      exit(1);
			    }



			    	potato_received=count;
			    	child_ended++;
				   }else if(potato_received>0){

				   	if(child_ended==n_players)
				   	break;
				   	
				   }


				  }
				}
			 }
			 //if(break_flag==1)
			 //	break;
			 
			}

		}

	// terminate all child
	count=0;
	tmp_buf=tmp_buf+2;
	int c_count=strlen(tmp_buf);
	int c_changecount=0;
	
	while(c_changecount<c_count){
	
	if(tmp_buf[c_changecount]==':')
		tmp_buf[c_changecount]=',';
		c_changecount++;
	}	
	
	printf("\nTrace of Potato: \n %s \n",tmp_buf);


	tmp_buf=malloc(100);
	while(count<child_index){

		//if(count!=potato_received){
				temp_send_len = recv(list[count].fd,tmp_buf, 100, 0);
			    if ( temp_send_len != strlen(tmp_buf) ) {
			      perror("send");
			      exit(1);
			    }
			   // printf("\n%d\n",count);
		//	}
		count++;
	}



	/*
	temp_send_len = send(list[0].fd,str, strlen(str), 0);
			    if ( temp_send_len != strlen(str) ) {
			      perror("send");
			      exit(1);
			    }else{
			    	printf("\n sent \n");
			    }
	*/
	
/* *//*
      if(temp_list==NULL){
		printf("0");	        		
      	}
      	else{
      			
		      	while(temp_list->next!=NULL){
		      		temp_list=temp_list->next;
		      		count++;
		      		}
		      	printf("\n%d",count);
      	}
*/
//while(1){
//	printf("1");
//}

}

int get_random(int players){


int n;
n=(int)rand();
n=n%players;
return n;

}
