        program lab2
        real xmax,xmin,ymax,ymin,dx,dy,x,y,f,eps,temp,d,feps
        integer i,j,n,l,a,k
        open(1,FILE='C://FRAN/textdoc/input3.txt')
        read(1,*)xmin,xmax,dx,ymin,ymax,dy
        close(1)
               
        y=ymin
        x=xmin
        eps = 0.001 !precision
        feps = 0.00001 !precision for function
        
        
        open(2,FILE='C://FRAN/textdoc/output3.txt')
        if(dx.le.0)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(dy.le.0)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(xmax.le.xmin)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(ymax.le.ymin)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(abs(xmin).gt.9999)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(abs(ymin).gt.9999)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(abs(xmax).gt.9999)then
        write(2, 4)'invalid data'
        goto 40
        end if
        if(abs(ymax).gt.9999)then
        write(2, 4)'invalid data'
        goto 40
        end if
        a=int(max(abs(xmin),abs(xmax)))
        k=0
21      if(a.eq.0)goto 22
        a=a/10
        k=k+1
        goto 21
22      d=1.0/real((10**(4-k)))
        print *, d
        read(*,*)
        if(dx.lt.d)dx=d 
        
        a=int(max(abs(ymin),abs(ymax)))
        k=0
23      if(a.eq.0)goto 24
        a=a/10
        k=k+1
        goto 23
24      d=1.0/real((10**(4-k)))
        print *, d
        read(*,*)
        if(dy.lt.d)dy=d !dobavit' dlay y        
        
3       format (E12.4, $)
4       format (E12.4, $) 
6       format (E24.4, $)
7       format ('            |', $)
8       format ('|', $)
9       format ('-', $)
11      format ('-')
12      format ('|')
17      format ('|   error    ', $)
18      format ('inf', $)
19      format ('-inf', $)
        i=int((xmax-xmin)/dx)

        n=0 !vspomogatelnyj shchetchik granicy
        l=i*20 !kalibrovka dliny mezhdustrochnoj granicy        
        write(2, 7)
        if(abs(x).lt.1e-10)x=0
        if(x.gt.1e+10)then
        write(2, 4)'inf'
        goto 13
        end if
        
        if(x.lt.-1e+10)then
        
        write(2, 19)
        goto 13
        end if
        
        write(2, 4)x
13      x=x+dx


        
100     if(i.eq.0)goto 5
        write(2, 8)
        if(abs(x).lt.1e-10)x=0
        if(x.gt.1e+10)then
        write(2, 18)
        goto 14
        end if
        
        if(x.lt.-1e+10)then
        
        write(2, 19)
        goto 14
        end if
        
        write(2, 4)x
14      x=x+dx
        i=i-1
        goto 100
     
5       if((abs(((xmax-xmin)/dx)-(int((xmax-xmin)/dx)))).gt.eps)then
        write(2, 3)xmax
        end if
        write(2, 12) !right line
        
        x=xmin
99      if(n.le.l)then
        write(2, 9)
        n=n+1
        goto 99
        end if        
        write(2, 11)
        n=0
          
        i=int((xmax-xmin)/dx)+1
        j=int((ymax-ymin)/dy)+1
1       if(j.eq.0)goto 10
        
        if(abs(y).lt.1e-10)y=0
        if(y.gt.1e+10)then
        write(2, 18)
        goto 16
        end if
        
        if(y.lt.-1e+10)then
        
        write(2, 19)
        goto 16
        end if
        
        write(2, 4)y

16      j=j-1
2       if(i.eq.0)goto 20
        if((abs(sin(x*3.1415926/180)+cos(y*3.1415926/180))).lt.feps)then
        write(2, 17)
        else 
        f=1/(sin(x*3.1415926/180)+cos(y*3.1415926/180))
        write(2, 8)
        write(2, 4)f        
        end if
        x=x+dx
        i=i-1
        goto 2
20      if((abs(((xmax-xmin)/dx)-(int((xmax-xmin)/dx)))).gt.eps)then
        temp=sin(xmax*3.1415926/80)+cos(y*3.1415926/180)
        if((abs(temp)).lt.feps)then
        write(2, 17)
        else 
        f=1/(sin(xmax*3.1415926/180)+cos(y*3.1415926/180))
        write(2, 3)f
        end if
        end if
        write(2, 12) !right line
        y=y+dy
        x=xmin
98      if(n.le.l)then
        write(2, 9)
        n=n+1
        goto 98
        end if 
        n=0
        write(2, 11)
        i=int((xmax-xmin)/dx)+1
        goto 1
10      if(  (abs(((ymax-ymin)/dy)-(int((ymax-ymin)/dy)))).gt.eps  )then
        write(2, 4)ymax
        else 
        goto 40
        end if
15      if(i.eq.0)goto 30
        temp=sin(x*3.1415926/180)+cos(ymax*3.1415926/180)
        if((abs(temp)).lt.feps)then
        write(2, 17)
        else 
        f=1/(sin(x*3.1415926/180)+cos(ymax*3.1415926/180))
        write(2, 8)
        write(2, 4)f        
        end if
        x=x+dx
        i=i-1
        goto 15
30      if(  (abs(((xmax-xmin)/dx)-(int((xmax-xmin)/dx)))).gt.eps  )then
        temp=sin(xmax*3.1415926/180)+cos(ymax*3.1415926/180)
        if((abs(temp)).lt.feps)then
        write(2, 17)
        else 
        f=1/(sin(xmax*3.1415926/180)+cos(ymax*3.1415926/180))
        write(2, 3)f
        end if
        end if
        write(2, 12) !right line
        
40      end
        
        
