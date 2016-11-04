
#启动tracker,storage程序
sudo /usr/bin/fdfs_trackerd ./conf/tracker.conf
sudo /usr/bin/fdfs_storaged ./conf/storage.conf


#启动redis服务器
redis-server ./conf/redis.conf
