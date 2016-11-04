#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>	

#include <hiredis.h>

#include "redis_op.h"
//#include "fcgi_stdio.h"
#include "util_cgi.h"
//#include "fcgi_config.h"

int main ()
{
    char *file_buf = NULL;
    char boundary[256] = {0};
    char content_text[256] = {0};
    char filename[256] = {0};
    char fdfs_file_path[256] = {0};
    char fdfs_file_stat_buf[256] = {0};
    char fdfs_file_host_name[30] = {0};
    char fdfs_file_url[512] = {0};
    //char *redis_value_buf = NULL;
    //time_t now;;
    //char create_time[25];
    //char suffix[10];
    


    while (FCGI_Accept() >= 0) {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        printf("Content-type: text/html\r\n"
                "\r\n");

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            printf("No data from standard input\n");
        }
        else {
            int i, ch;
            char *begin = NULL;
            char *end = NULL;
            char *p, *q, *k;

            //==========> 开辟存放文件的 内存 <===========

            file_buf = malloc(len);
            if (file_buf == NULL) {
                printf("malloc error! file size is to big!!!!\n");
                return -1;
            }

            begin = file_buf;
            p = begin;

            for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
                }
                //putchar(ch);
                *p = ch;
                p++;
            }

            //===========> 开始处理前端发送过来的post数据格式 <============
            //begin deal
            end = p;

            p = begin;

            //get boundary
            p = strstr(begin, "\r\n");
            if (p == NULL) {
                printf("wrong no boundary!\n");
                goto END;
            }

            strncpy(boundary, begin, p-begin);
            boundary[p-begin] = '\0';
            //printf("boundary: [%s]\n", boundary);

            p+=2;//\r\n
            //已经处理了p-begin的长度
            len -= (p-begin);

            //get content text head
            begin = p;

            p = strstr(begin, "\r\n");
            if(p == NULL) {
                printf("ERROR: get context text error, no filename?\n");
                goto END;
            }
            strncpy(content_text, begin, p-begin);
            content_text[p-begin] = '\0';
            //printf("content_text: [%s]\n", content_text);

            p+=2;//\r\n
            len -= (p-begin);

            //get filename
            // filename="123123.png"
            //           ↑
            q = begin;
            q = strstr(begin, "filename=");
            
            q+=strlen("filename=");
            q++;

            k = strchr(q, '"');
            strncpy(filename, q, k-q);
            filename[k-q] = '\0';

            trim_space(filename);
            //printf("filename: [%s]\n", filename);

            //get file
            begin = p;     
            p = strstr(begin, "\r\n");
            p+=4;//\r\n\r\n
            len -= (p-begin);

            begin = p;
            // now begin -->file's begin
            //find file's end
            p = memstr(begin, len, boundary);
            if (p == NULL) {
                p = end-2;    //\r\n
            }
            else {
                p = p -2;//\r\n
            }
        
            //begin---> file_len = (p-begin)
            int fd = 0;
            fd = open(filename, O_CREAT|O_WRONLY, 0644);
            if (fd < 0) {
                printf("open %s error\n", filename);

            }

            ftruncate(fd, (p-begin));
            write(fd, begin, (p-begin));
            close(fd);

            //=====> 此时begin-->p两个指针的区间就是post的文件二进制数据

            //======>将数据写入文件中,其中文件名也是从post数据解析得来  <===========


            //===============> 将该文件存入fastDFS中,并得到文件的file_id <============
			char file_id[1024] = {0};
			pid_t pid;
		    int pfd[2];//管道fd
		    if (pipe(pfd) < 0) {
		        //LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "pip error");
		        printf("func pipe() err\n");
		        exit(1);
		    }
		    pid = fork();
		    if (pid == 0) {
		        //child
		        //关闭读端
		        close(pfd[0]);
		        //将stdout ---》pfd[1]
		        dup2(pfd[1], STDOUT_FILENO);
		
		        //exec
		        execlp("fdfs_upload_file", "fdfs_upload_file", "./conf/client.conf", filename, NULL);
		        //LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "exec error");
		        
		    }
		    else {
		        //parent
		        close(pfd[1]);
		
		        wait(NULL);
		
		        //从管道读数据
		        read(pfd[0], file_id, 1024);
		
		
		        //LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "upload file_id[%s] succ!", file_id);
		        printf("file_id:%s\n",file_id);
		    }	
		    //writer redis
		    int ret=0;
		    redisContext*handle;
		    //char key_value[100]={0};
		    handle=rop_connectdb_nopwd("127.0.0.1", "6379");
		    if(handle==NULL)
			{
				ret=-2;
				printf("func rop_connectdb_nopwd err:%d\n",ret);
				return ret;
			}
			int file_id_len=strlen(file_id);
			file_id[file_id_len-1]='\0';
		    ret=rop_set_string(handle, file_id, filename);
			if(ret==-1)
			{
				printf("func rop_set_string err:%d\n",ret);
				goto END;
			}
			
            //================ > 得到文件所存放storage的host_name <=================
END:
			rop_disconnect(handle);
            memset(boundary, 0, 256);
            memset(content_text, 0, 256);
            memset(filename, 0, 256);
            memset(fdfs_file_path, 0, 256);
            memset(fdfs_file_stat_buf, 0, 256);
            memset(fdfs_file_host_name, 0, 30);
            memset(fdfs_file_url, 0, 512);

            free(file_buf);
            //printf("date: %s\r\n", getenv("QUERY_STRING"));
        }
    } /* while */

	
	
	
    return 0;
}
