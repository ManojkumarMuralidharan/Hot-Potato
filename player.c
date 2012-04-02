#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


int main(int argc, char **argv){
	
	int s1, s2, n,server_port,sc1,sc2,rv,rc,left_peer_port;
	int peer_port;
	fd_set writefds,readfds;
	struct timeval tv;
	struct hostent* server_info,*child_info,*left_peer_info;
	int start_flag=0;
	int errno=-1;
	int total_players;

	struct sockaddr_in server,peer,incoming;

	char host_child[64];
	char *left_host=malloc(1);
	char *right_host=malloc(1);
	char buf1[256];
	char buf2[256];

	if(argv[1]==NULL){
	perror("Provide Master machine name. speak <Master m/c name> <port>");
	exit(1);
	}
	if(argv[2]==NULL){
	perror("Provide Master port. speak <Master m/c name> <port>");
	exit(1);
	}

	server_port=atoi(argv[2]);

	//speak to peer using this
	s2 = socket(AF_INET, SOCK_STREAM, 0);
	if ( s2 < 0 ) {
    perror("socket:");
    exit(s2);
  	}
  	
	peer_port=server_port+1;

	gethostname(host_child, sizeof host_child);

	child_info=gethostbyname(host_child);

	if ( child_info == NULL ) {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host_child);
    exit(1);
	}

	/* open a socket for listening
   * 4 steps:
   *	1. create socket - done
   *	2. bind it to an address/port
   *	3. listen
   *	4. accept a connection
   */

	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = htonl(INADDR_ANY);
	memcpy(&peer.sin_addr, child_info->h_addr_list[0], child_info->h_length);
	int changeport=0;
   do
  	{
	    peer.sin_port = htons(peer_port);
	  	rc = bind(s2, (struct sockaddr *)&peer, sizeof(peer));

		if ( rc < 0 ) {
	    	//perror("bind:");
	    	peer_port++;
	    	changeport=1;
	    	//exit(rc);

	  	}else{
	  		changeport=0;
	  	}

    }while(changeport==1);

	  if (listen(s2, 100) == -1) {
        perror("listen");
        exit(3);
      }



    //Now talk to server

	server_info= gethostbyname(argv[1]); 

	if ( server_info == NULL ) {
    fprintf(stderr, "%s: host not found\n", argv[0]);
    exit(1);
  	}
	

	// pretend we've connected both to a server at this point
	//speak to server using this
	s1 = socket(AF_INET, SOCK_STREAM, 0);
	if ( s1 < 0 ) {
    perror("socket:");
    exit(s1);
  	}
		

	server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
  	memcpy(&server.sin_addr, server_info->h_addr_list[0], server_info->h_length);
  	
  	//To talk to the server
  	/* connect to socket at above addr and port */
	  sc1 = connect(s1, (struct sockaddr *)&server, sizeof(server));
	  if ( sc1 < 0 ) {
	    perror("connect:");
	    exit(sc1);
	  }

	int send_len;
	char *str=malloc(7);
	  if ( str == NULL ) {
   		 perror("malloc");
   		 exit(errno);
    }
	sprintf(str,"%d",peer_port);
	//printf("\nport-here=%s\n",str);



	send_len = send(s1, str, strlen(str), 0);
		    if ( send_len != strlen(str) ) {
		      perror("send");
		      exit(1);
		    }

	int recv_len;
	char buf[100];
	int ref=0;
	int recv_stage=0;
	int start_id;

	

    //while ( recv_stage<2 ) {

	      recv_len = recv(s1, buf, 64, 0);
	      if ( recv_len < 0 ) {
				perror("recv");
				exit(1);
		   }
	      buf[recv_len] = '\0';
	      char *temp_port;
	      //printf("\n-%s\n",buf);
	      if ( !strcmp("close_string", buf) )
	      		printf("%s",buf);
			//break;
	      else{
	      	char *temp_buffer=malloc(100);
	      	  if ( temp_buffer == NULL ) {
    			perror("malloc");
    			exit(errno);
  			}
	      	char *id=malloc(100);
	      	  if ( id == NULL ) {
    			perror("malloc");
    			exit(errno);
  			}
	      	char *pp=malloc(6);
	      	  if ( pp == NULL ) {
    			perror("malloc");
    			exit(errno);
  			}
	      	strcpy(temp_buffer,buf);
	      	
	      	char *search=":";
	      	temp_port=strtok(temp_buffer,search);
	      	strcpy(buf,temp_port);
	      //printf("temp_name%s",temp_port);
	      	//buf[strlen(buf)-strlen(temp_port)-1]='\0';
	      	temp_port=strtok(NULL,search);
	      	strcpy(pp,temp_port);
	      //	printf("temp_port%s",temp_port);
	      	temp_port=strtok(NULL,search);
	      	strcpy(id,temp_port);
	      	printf("\nConnected as player %s\n",id);
	      	temp_port=strtok(NULL,search);
		total_players=atoi(temp_port);
		//printf("\nTotal no of player %d\n",total_players);
	      	//id=strtok(NULL,search);
	      	strcpy(temp_port,pp);
	      //	printf("\nbuf=%s\n",buf);
	     // 	printf("\nport=%s\n",temp_port);
	      //	printf("\nid=%s\n",id);
	      	start_id=atoi(id);
		
	       }

	       //To send packet to its neighbour
	       left_peer_info=gethostbyname(buf);
	      // printf("\nleft host=%s\n",buf);
					if ( left_peer_info == NULL ) {
				    fprintf(stderr, "%s: host not found\n",left_host);
				    exit(1);
					}
		   left_peer_port=atoi(temp_port);


		int neighbour_socket;
		struct hostent *neighbour_info;
		struct sockaddr_in neighbour;
		neighbour_socket = socket(AF_INET, SOCK_STREAM, 0);
		if ( neighbour_socket < 0 ) {
    		perror("socket:");
    		exit(neighbour_socket);
  		}

  		neighbour_info=left_peer_info;
		

		neighbour.sin_family = AF_INET;
	    neighbour.sin_port = htons(left_peer_port);
	    neighbour.sin_addr.s_addr = htonl(INADDR_ANY);
	  	memcpy(&neighbour.sin_addr, neighbour_info->h_addr_list[0], neighbour_info->h_length);
  		
  		//printf("\nconnected to port %d\n",left_peer_port);
  		//To talk to the neighbour
  		/* connect to socket at above addr and port */

    //printf("\ncomes here\n");


    //Listen on this socket connection for potato
	
	 
	
	
	int send_peer_len;


	int nfds;

	nfds=MAX(MAX(neighbour_socket,s2),s1)+1;


	

	// clear the set ahead of time
	FD_ZERO(&writefds);

	// add our descriptors to the set
	FD_SET(neighbour_socket,&writefds);
    FD_SET(s1,&writefds);
    FD_SET(s2,&writefds);

	//FD_SET(s2, &writefds);


	FD_ZERO(&readfds);
    //FD_SET(neighbour_socket,&readfds);
    FD_SET(s1,&readfds);
    FD_SET(s2,&readfds);



	tv.tv_sec = 10;
	tv.tv_usec = 500000;

	int fds[3];
	fds[0]=s1;
	fds[1]=s2;
	fds[2]=neighbour_socket;
	int p=0;
	int p_flag=0;
	int connect_flag=0;
	int msg_size;
	char *sizemsg;
	char *hot_potato;
	char *temp_hot_potato;

	int size_flag=1;
	int msg_flag=0;

	int idsize=0;
	int tempid=start_id;
	int end_flag=0;
	int release_neighbour=0;
	int end_game=0;

	while(tempid>0){
		idsize++;
		tempid=tempid/10;
	}

	while(1){
		//printf("\n nfds = %d , p = %d \n",nfds,p);
		char buf_msg[64];
		rv = select(nfds,&readfds,  NULL, NULL, 0);

		if (rv == -1) {
		    perror("select"); // error occurred in select()
		} else if (rv == 0) {
		    printf("Timeout occurred!  No data after 10.5 seconds.\n");
		} else {
		    // one or both of the descriptors have data
		    //reading message from server
		    int select_fd;
		    for(select_fd=0;select_fd<=nfds;select_fd++){

		    if(FD_ISSET(select_fd,&readfds)!=0){	

			    if(select_fd==s1){
			    	//printf("\nFrom server\n");

			    	if(connect_flag==0){

			    	recv_len=0;
			  		recv_len = recv(s1, buf_msg, 34, 0);
			      	if ( recv_len < 0 ) {
						perror("recv");
						exit(1);
				   	}
		      		buf_msg[recv_len] = '\0';
		      	//	printf("\nreceived from server = %s\n",buf_msg);
			    	//send to neighbour
			    	
				int sc3;
				if(start_flag==0){
			  	sc3 = connect(neighbour_socket, (struct sockaddr *)&neighbour, sizeof(neighbour));
			  	if ( sc3 < 0 ) {
			    	perror("connect:");
				    exit(sc3);

				}
				start_flag=1;
				}
				//connect_flag=1;

					str=malloc(recv_len+1);
					  if ( str == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
			    	strcpy(str,buf_msg);
		    		send_len = send(neighbour_socket, str, strlen(str), 0);
		    	//	printf("\n send - len - s1  - %d\n",send_len);
				    if ( send_len != strlen(str) ) {
				      perror("send");
				      exit(1);
				    }
				  }else if(connect_flag==1){
	  	//printf("\nFrom server\n");
				  		recv_len=0;
				  		int n;
				  		int hop_count;
				  		char *hops;
				  		char *temp_str;
				  		if(ioctl(s1, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
 						 perror("ioctl");
  						 exit(1);
						}
			//			printf("\n %d Bytes available for read ++\n",n);
			//			printf("\nI'm It\n");
						sizemsg=malloc(n+1);
						  if ( sizemsg == NULL ) {
    							perror("malloc");
    							exit(errno);
  						}
						//memset(sizemsg,'\0',sizeof(sizemsg)-1);
				  		recv_len = recv(s1, sizemsg, n, 0);
				      	if ( recv_len < 0 ) {
							perror("recv");
							exit(1);
					   	}
					   sizemsg[recv_len]='\0';
					   char *close_string="endgame";
					   if(strcmp(sizemsg,close_string)==0){
					   	//printf("Child %d ends",start_id);
					   	//close_string(s2);
					   	//close_string(s1);
						//printf("\nSending Potato to %d\n",(start_id==total_players)?1:start_id+1);
					   	send_len = send(neighbour_socket, close_string, strlen(close_string), 0);
		    	//		printf("\n send - len - s1  -size sent %s\n",close_string);
				    	if ( send_len != strlen(close_string) ) {
				      		perror("send");
				      		exit(1);
				    	}
				    	end_game=1;
				    	/*send_len = send(p, close_string, strlen(close_string), 0);
		    			printf("\n send - len - s1  - %d -size sent %s\n",close_string);
				    	if ( send_len != strlen(close_string) ) {
				      		perror("send");
				      		exit(1);
				    	}*/
				    	//close_string(neighbour_socket);
					   	//exit(0);
					   }else{
					   temp_str=malloc(strlen(sizemsg)+1);
					     if ( temp_str == NULL ) {
    						perror("malloc");
    						exit(errno);
  						}
					   //memset(temp_str,'\0',sizeof(temp_str)-1);
					   strcpy(temp_str,sizemsg);
					   hops=strtok(temp_str,":");
					   


			//		   printf("\nhops=%s\n",hops);
					   hop_count=atoi(hops);
					   sizemsg=strtok(NULL,":");
			//	  	printf("\nfinal message = %s\n",sizemsg);
		
					char *temp_str1;
					int temp_len;
					temp_len=n+idsize+2;
				  	hot_potato=malloc(temp_len);
				  	  if ( hot_potato == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
				  	
				  	if(sizemsg==NULL)
		      		sprintf(hot_potato,"%d:%d",hop_count-1,start_id);
		      		else{
		      		temp_str1=malloc(strlen(sizemsg) - strlen(hops));
		      		  if ( temp_str1 == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
		      		
				  	strcpy(temp_str1,sizemsg+strlen(hops)+1);
		      		
		      		sprintf(hot_potato,"%d:%s:%d",hop_count-1,temp_str1,start_id);
		      		//printf("\n hot potato = %s \n",hot_potato);
		      		}
		      		
		      		if(hop_count==1){
		      		
		      		send_len = send(s1, hot_potato, strlen(hot_potato), 0);
	//	    		printf("\n send - len - s1  - %d -size sent %s\n",send_len,hot_potato);
				    if ( send_len != strlen(hot_potato) ) {
				      perror("send");
				      exit(1);
				    }



		      		}else{
			      		//printf("\n hot potato = %s \n",hot_potato);
						printf("\nSending Potato to %d\n",(start_id==total_players)?1:start_id+1);
			      		send_len = send(neighbour_socket, hot_potato, strlen(hot_potato), 0);
			    //		printf("\n send - len - s1  - %d -size sent %s\n",send_len,hot_potato);
					    if ( send_len != strlen(hot_potato) ) {
					      perror("send");
					      exit(1);
					    }
					}
				    free(sizemsg);
				    free(temp_str);



				  }
				
			}
			    	
			    }else if (select_fd==s2) {
		//	    	printf("\nOn its socket\n");

			    	if(connect_flag==0){
			    	
					int list_len=sizeof incoming;
					//printf("\ninside client\n");
					//printf("\nWaiting for accept in S2\n");
					if(p_flag==0){
						p_flag=1;
						p = accept(s2, (struct sockaddr *)&incoming, &list_len);
						/*
						int p_buffersize = 1024*1024* 1024 ;
						if (setsockopt(p, SOL_SOCKET, SO_RCVBUF, &p_buffersize, sizeof(p_buffersize)) == -1) {
  						// deal with failure, or ignore if you can live with the default size
  						printf("\nDefault p buffers size\n");
						}
						if (setsockopt(p, SOL_SOCKET, SO_SNDBUF, &p_buffersize, sizeof(p_buffersize)) == -1) {
  						// deal with failure, or ignore if you can live with the default size
  						printf("\nDefault p buffers size\n");
						}
						if (setsockopt(neighbour_socket, SOL_SOCKET, SO_SNDBUF, &p_buffersize, sizeof(p_buffersize)) == -1) {
  						// deal with failure, or ignore if you can live with the default size
  						printf("\nDefault neighbour buffers size\n");
						}
						*/

						connect_flag=1;
						//printf("\nConnection accepted in S2\n");
					    if ( p < 0 ) {
					      perror("bind:");
					      exit(rc);
					    }
					}

				    nfds=MAX(nfds,p)+1;
			    	recv_len=0;
			    	buf_msg[recv_len] = '\0';


			  		recv_len = recv(p, buf_msg, 34, 0);
			      	if ( recv_len < 0 ) {
						perror("recv");
						exit(1);
				   	}
		      		buf_msg[recv_len] = '\0';
		  //    		printf("\nreceived from client = %s\n",buf_msg);
			    	//send to neighbour
	
				    if(start_flag!=1){
					    int sc3;
				  		sc3 = connect(neighbour_socket, (struct sockaddr *)&neighbour, sizeof(neighbour));
				  		if ( sc3 < 0 ) {
				    	perror("connect:");
					    //exit(sc3);
						}


					}else{
					char *conn_msg;
					conn_msg=malloc(16);
					conn_msg="connectiondone";
					str=malloc(16);
					  if ( str == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
			    	strcpy(str,conn_msg);
		    		send_len = send(s1, str, strlen(str), 0);
		    		//printf("\n send - len - s2  - %d\n",send_len);
				    if ( send_len != strlen(str) ) {
				      perror("send");
				      exit(1);
				    }	

					}

					
					str=malloc(recv_len+1);
					  if ( str == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
			    	strcpy(str,buf_msg);
		    		if(start_flag!=1){
			    		send_len = send(neighbour_socket, str, strlen(str), 0);
			    		//printf("\n send - len - s2  - %d\n",send_len);
					    if ( send_len != strlen(str) ) {
					      perror("send");
					      exit(1);
					    }
					}
				    
					
					
				    
				    FD_CLR(s2, &readfds);
				}
				else if(connect_flag==1){


		//			printf("inside connection");
						recv_len=0;
				  		int n;
				  		int hop_count;
				  		char *hops;
				  		char *temp_str;
				  		if(ioctl(p, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
 						 perror("ioctl");
  						 exit(1);
						}
		//				printf("\n %d Bytes available for read \n",n);
						sizemsg=malloc(n+1);
						  if ( sizemsg == NULL ) {
    						perror("malloc");
    						exit(errno);
  						}
						//memset(sizemsg,'\0',sizeof(sizemsg)-1);
				  		recv_len = recv(p, sizemsg, n, 0);
				      	if ( recv_len < 0 ) {
							perror("recv");
							exit(1);
					   	}
					   sizemsg[recv_len]='\0';
					   temp_str=malloc(strlen(sizemsg)+1);
					     if ( temp_str == NULL ) {
    						perror("malloc");
   							exit(errno);
  						}
					   //memset(temp_str,'\0',sizeof(temp_str)-1);
					   strcpy(temp_str,sizemsg);
					   hops=strtok(temp_str,":");
					   hop_count=atoi(hops);
		//			   printf("\nhops=%s\n",hops);
	//				   printf("\ntemp str=%s\n",temp_str);
	//			  	printf("\nfinal message = %s\n",sizemsg);
				  		
				  	hot_potato=malloc(n+idsize+2);
				  	  if ( hot_potato == NULL ) {
    						perror("malloc");
    						exit(errno);
  						}
				  	//memset(hot_potato,'\0',sizeof(hot_potato)-1);
				  	char *temp_str1;
				  	if(sizemsg==NULL)
		      		sprintf(hot_potato,"%d:%d",hop_count-1,start_id);
		      		else{
		      		temp_str1=malloc(strlen(sizemsg) - strlen(hops));
		      		  if ( temp_str1 == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
		      		//memset(temp_str1,'\0',sizeof(temp_str1)-1);
				  	strcpy(temp_str1,sizemsg+strlen(hops)+1);
		      		//sizemsg=sizemsg+strlen(hops)+1;
		      		sprintf(hot_potato,"%d:%s:%d",hop_count-1,temp_str1,start_id);
	//	      		printf("\n hot potato = %s \n",hot_potato);
		      		}
		      		

	//	      		printf("\n hot potato = %s \n",hot_potato);

		      		if(hop_count==1){
		      		send_len = send(s1, hot_potato, strlen(hot_potato), 0);
	//	    		printf("\n send - len - s1  - %d -size sent %s\n",send_len,hot_potato);
				    if ( send_len != strlen(hot_potato) ) {
				      perror("send");
				      exit(1);
				    }	


		      		}else if(hop_count>1){
		      		


		      		send_len = send(neighbour_socket, hot_potato, strlen(hot_potato), 0);
	//	    		printf("\n send - len - neighbouring socket  - %d -size sent %s\n",send_len,hot_potato);
				    if ( send_len != strlen(hot_potato) ) {
				      perror("send");
				      exit(1);
				    }		      			

		      		}



				   free(sizemsg);
				   free(temp_str1);
				   free(temp_str);


				}



			    }else if(select_fd==neighbour_socket){
			    
	//		    	printf("\ncomes in Neighbouring socket\n");



			    	recv_len=0;
				  		int n;
				  		int hop_count;
				  		char *hops;
				  		char *temp_str;
				  		int read_flag=0;


				  		do{
				  		if(ioctl(neighbour_socket, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
 						 perror("ioctl");
  						 exit(1);
						}
						//printf("\nn=%d\n",n);
						if(n>0){

							if(read_flag==0){
							//printf("\n %d Bytes available for read \n",n);
							//temp_n=temp_n+n;
							//printf("\nN=%d,tempN=%d",n,temp_n);
							if(n>1024)
							n=1024;
							sizemsg=malloc(n+1);
							  if ( sizemsg == NULL ) {
	    						perror("malloc");
	    						exit(errno);
	  						}
							//memset(sizemsg,'\0',sizeof(sizemsg)-1);
					  		recv_len = recv(neighbour_socket, sizemsg, n, 0);
					      	if ( recv_len < 0 ) {
								perror("recv");
								exit(1);
						   	}
						   	sizemsg[recv_len]='\0';
						   	read_flag=1;
						   	//printf("\nread flag set %d \n",read_flag);

						   }else if(read_flag==1){
						   //	printf("\ncomes in to here read flag set %d \n",read_flag);
						   	char *temp_sizemsg;
						   	int new_size=0;
						   	if(n>1024)
							n=1024;
						   	new_size=strlen(sizemsg)+n+1;
						   	temp_sizemsg=malloc(strlen(sizemsg)+n+1);
						   	strcpy(temp_sizemsg,sizemsg);
						   	free(sizemsg);
						   	sizemsg=malloc(n+1);
							if ( sizemsg == NULL ) {
	    						perror("malloc");
	    						exit(errno);
	  						}
							//memset(sizemsg,'\0',sizeof(sizemsg)-1);
					  		recv_len = recv(neighbour_socket, sizemsg, n, 0);
					      	if ( recv_len < 0 ) {
								perror("recv");
								exit(1);
						   	}
						   	//printf("\nsecond receive=%d\n",recv_len);
						   	sizemsg[recv_len]='\0';
						   	strcat(temp_sizemsg,sizemsg);
						   	free(sizemsg);
						   	sizemsg=malloc(strlen(temp_sizemsg)+1);
						   	strcpy(sizemsg,temp_sizemsg);
						   	free(temp_sizemsg);
						   	
						   }
					   	}
					}while(n>0);


						int new_size=0;
						new_size=strlen(sizemsg);
					   	//printf("\n crossed \n");
					   sizemsg[new_size]='\0';
	//				   printf("\nReceived len=%d \n",strlen(sizemsg));
					   char *close_string="endgame";
					   if(strcmp(sizemsg,close_string)==0){
					   //	printf("Child %d ends",start_id);
					   //	close_string(s2);
					   //	close_string(s1);
					   release_neighbour=1;
					   	close(neighbour_socket);
					   	//exit(0);
					   }else{


					   int temp_len=0;
					   temp_len=strlen(sizemsg)+1;
					   temp_str=malloc(temp_len);
					     if ( temp_str == NULL ) {
    						perror("malloc");
    						exit(errno);
  						}
					   //memset(temp_str,'\0',sizeof(temp_str)-1);
					   strcpy(temp_str,sizemsg);




					   hops=strtok(temp_str,":");
					   char *tt;
					   tt=strtok(NULL,"\0");
					  // printf("\nremainin-%s\n",tt);
					   hop_count=atoi(hops);

					   //printf("\ntemp str=%s\n",temp_str);
	//				   printf("\nhops=%s\n",hops);
	//			  	
				  		
				  	hot_potato=malloc(new_size+idsize+2);
				  	  if ( hot_potato == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
				  	//memset(hot_potato,'\0',sizeof(hot_potato)-1);
				  	char *temp_str1;

				  	if(sizemsg==NULL)
		      		sprintf(hot_potato,"%d:%d",hop_count-1,start_id);
		      		else{
		      			int temp_len=strlen(sizemsg) - strlen(hops);
		      		temp_str1=malloc(temp_len);
		      		  if ( temp_str1 == NULL ) {
  						  	 perror("malloc");
   							 exit(errno);
  						}
		      		strncpy(temp_str1,tt,strlen(tt));
				  	int tlen=strlen(tt);
		      		
		      		sprintf(hot_potato,"%d:%.*s:%d",hop_count-1,tlen,temp_str1,start_id);
		      		
		      		}
		      		

		      		if(hop_count==1){
		      		send_len = send(s1, hot_potato, strlen(hot_potato), 0);
		    		//printf("\n send - len - s1  - %d -size sent %s\n",send_len,hot_potato);
					    if ( send_len != strlen(hot_potato) ) {
					      perror("send");
					      exit(1);
					    }
		      		}else if(hop_count>1){

		      		int choice;
		      		choice=get_random(2);
		      		if(choice==0){
				

				printf("\nSending Potato to %d\n",(start_id==total_players)?1:start_id+1);
		      		send_len = send(neighbour_socket, hot_potato, strlen(hot_potato), 0);
				}
		      		else{
				printf("\nSending Potato to %d\n",((start_id-1)==0)?total_players:(start_id-1));				
		      		send_len = send(p, hot_potato, strlen(hot_potato), 0);
				}

				    if ( send_len != strlen(hot_potato) ) {
				      perror("send");
				      exit(1);
				    }
				    //printf("\n Send Length=%d",send_len);		      			

		      		}
				    free(sizemsg);
				    free(temp_str1);
				    free(hot_potato);
				    free(temp_str);

				}












			    	
			    }else if( select_fd==p && p!=0) {
			    	//printf("\nFrom P\n");

			    	if(connect_flag==0){

			    	int list_len=sizeof incoming;
					//printf("\ninside p\n");
					//p = accept(s2, (struct sockaddr *)&incoming, &list_len);
				    //if ( p < 0 ) {
				    //  perror("bind:");
				    //  exit(rc);




				    //}
			    	recv_len=0;
			    	buf_msg[recv_len] = '\0';
			    	//printf("\nwaiting at recv in P\n");

			  		recv_len = recv(p, buf_msg, 34, 0);

			  		//printf("\ncrossed at recv in P\n");
			      	if ( recv_len < 0 ) {
						perror("recv");
						exit(1);
				   	}
		      		buf_msg[recv_len] = '\0';
		      		//printf("\nreceived from client in p= %s\n",buf_msg);

			    	//send to neighbour
			    	str=malloc(1);
			    	  if ( str == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
			    	strcpy(str,buf_msg);
		    		send_len = send(neighbour_socket, str, strlen(str), 0);
		    		//printf("\n send - len - neighbour - %d\n",send_len);
				    if ( send_len != strlen(str) ) {
				      perror("send");
				      exit(1);
				    }
				    
				}else if(connect_flag==1){
						recv_len=0;
				  		int n;
				  		int hop_count;
				  		char *hops;
				  		char *temp_str;
				  		int read_flag=0;


				  		do{
				  		if(ioctl(p, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
 						 perror("ioctl");
  						 exit(1);
						}
						//printf("\nn=%d\n",n);
						if(n>0){

							if(read_flag==0){
							//printf("\n %d Bytes available for read \n",n);
							//temp_n=temp_n+n;
							//printf("\nN=%d,tempN=%d",n,temp_n);
							if(n>16000)
							n=16000;
							sizemsg=malloc(n+1);
							  if ( sizemsg == NULL ) {
	    						perror("malloc");
	    						exit(errno);
	  						}
							//memset(sizemsg,'\0',sizeof(sizemsg)-1);
							int t;
							t=n;

					  		recv_len = recv(p, sizemsg, n, 0);
					      	if ( recv_len < 0 ) {
								perror("recv");
								exit(1);
						   	}
						   	sizemsg[recv_len]='\0';
						   	read_flag=1;
						   	//printf("\nread flag set %d \n",read_flag);

						   }else if(read_flag==1){
						   	//printf("\ncomes in to here read flag set %d \n",read_flag);
						   	char *temp_sizemsg;
						   	int new_size=0;
						   	new_size=strlen(sizemsg)+1+1;
						   	temp_sizemsg=malloc(strlen(sizemsg)+1+1);
						   	strcpy(temp_sizemsg,sizemsg);
						   	free(sizemsg);
						   	sizemsg=malloc(2);
							if ( sizemsg == NULL ) {
	    						perror("malloc");
	    						exit(errno);
	  						}
							//memset(sizemsg,'\0',sizeof(sizemsg)-1);
					  		recv_len = recv(p, sizemsg, 1, 0);
					      	if ( recv_len < 0 ) {
								perror("recv");
								exit(1);
						   	}
						   	//printf("\nsecond receive=%d\n",recv_len);
						   	sizemsg[recv_len]='\0';
						   	strcat(temp_sizemsg,sizemsg);
						   	free(sizemsg);
						   	sizemsg=malloc(strlen(temp_sizemsg)+1);
						   	strcpy(sizemsg,temp_sizemsg);
						   	free(temp_sizemsg);
						   	
						   }
					   	}
					}while(n>0);


						int new_size=0;
						new_size=strlen(sizemsg);
					   	//printf("\n crossed \n");
					   sizemsg[new_size]='\0';
					   //printf("\nReceived len=%d \n",strlen(sizemsg));
					   char *close_string="endgame";
					   if(strcmp(sizemsg,close_string)==0){
					   	//printf("Child %d ends",start_id);
					   //	close_string(s2);
					   //	close_string(s1);
					   //

				
				    	send_len = send(s1, close_string, strlen(close_string), 0);
		    			//printf("\n send - len - s1  - -size sent %s\n",close_string_string);
				    	if ( send_len != strlen(close_string) ) {
				      		perror("send");
				      		exit(1);
				    	}
				    	close(s1);

				    	//printf("\nEnd games is \n",end_game);
				    	if(end_game!=1){

					   	send_len = send(neighbour_socket, close_string, strlen(close_string), 0);
		    			//printf("\n send - len - s1  - -size sent %s\n",close_string);
				    	if ( send_len != strlen(close_string) ) {
				      		perror("send");
				      		exit(1);
				    	}
				    	close(neighbour_socket);
				    	}else{
					printf("\nI'm It\n");
					}
				    	
//				    	printf("\nsend length=^%d\n",send_len);
				     //	close_string(neighbour_socket);
					   	exit(0);
					   }
					   int temp_len=0;
					   temp_len=strlen(sizemsg)+1;
					   temp_str=malloc(temp_len);
					     if ( temp_str == NULL ) {
    						perror("malloc");
    						exit(errno);
  						}
					   //memset(temp_str,'\0',sizeof(temp_str)-1);
					   strcpy(temp_str,sizemsg);




					   hops=strtok(temp_str,":");
					   char *tt;
					   tt=strtok(NULL,"\0");
					  // printf("\nremainin-%s\n",tt);
					   hop_count=atoi(hops);

					   //printf("\ntemp str=%s\n",temp_str);
					   //printf("\nhops=%s\n",hops);
				  	
				  		
				  	hot_potato=malloc(new_size+idsize+2);
				  	  if ( hot_potato == NULL ) {
    					perror("malloc");
    					exit(errno);
  					}
				  	//memset(hot_potato,'\0',sizeof(hot_potato)-1);
				  	char *temp_str1;

				  	if(sizemsg==NULL)
		      		sprintf(hot_potato,"%d:%d",hop_count-1,start_id);
		      		else{
		      			int temp_len=strlen(sizemsg) - strlen(hops);
		      		temp_str1=malloc(temp_len);
		      		  if ( temp_str1 == NULL ) {
  						  	 perror("malloc");
   							 exit(errno);
  						}
		      		strncpy(temp_str1,tt,strlen(tt));
				  	int tlen=strlen(tt);
		      		
		      		sprintf(hot_potato,"%d:%.*s:%d",hop_count-1,tlen,temp_str1,start_id);
		      		
		      		}
		      		

		      		if(hop_count==1){
		      		send_len = send(s1, hot_potato, strlen(hot_potato), 0);
		    		//printf("\n send - len - s1  - %d -size sent %s\n",send_len,hot_potato);
					    if ( send_len != strlen(hot_potato) ) {
					      perror("send");
					      exit(1);
					    }
		      		}else if(hop_count>1){

		      		int choice;
		      		choice=get_random(2);


		      		if(choice==0){
				printf("\nSending Potato to %d\n",(start_id==total_players)?1:start_id+1);

		      		send_len = send(neighbour_socket, hot_potato, strlen(hot_potato), 0);
				}
		      		else{
				printf("\nSending Potato to %d\n",((start_id-1)==0)?total_players:(start_id-1));				
		      		send_len = send(p, hot_potato, strlen(hot_potato), 0);
				}

				    if ( send_len != strlen(hot_potato) ) {
				      perror("send");
				      exit(1);
				    }
//				    printf("\n Send Length=%d",send_len);		      			

		      		}
				    free(sizemsg);
				    free(temp_str1);
				    free(hot_potato);
				    free(temp_str);

					
				}
		  	    	
			    }
			}
		}
		struct sockaddr_in incoming1;
		int list_len=sizeof incoming1;
		//int list_len;
		


		FD_ZERO(&readfds);
    				//FD_SET(neighbour_socket,&readfds);
    	FD_SET(s1,&readfds);
    	if(release_neighbour==0)
    	FD_SET(neighbour_socket,&readfds);
			
		if(p_flag==1){
			//printf("\nsetting P\n");
			FD_SET(p,&readfds);
			FD_SET(s2,&readfds);

			//p_flag=0;
		}else{
			//printf("\nNot setting P\n");
			FD_SET(s2,&readfds);
		}

		}
	}
	close_string(s1);
	close_string(s2);
	

}


int get_random(int players){


int n;
n=(int)rand();
n=n%players;
//printf("%d",n);
return n;

}
