- push时提示需输入密码：

  [mjh@rhel mystudy]$ git push origin master
  git@github.com's password:

  》》》》

  1. ssh-keygen：生成公钥，并将公钥拷贝到github上

  2. ssh-add ~/.ssh/id_rsa：添加生成的秘钥

     - 若出现Could not open a connection to your authentication agent，则可执行

       ```shell
       eval `ssh-agent -s`
       # 或清空known_hosts
       ```

  3. 再次执行push，即可成功！

  ​    