        program lab2
        common/storage/xmax,xmin,ymax,ymin,dx,dy,x,y,f
        open(1,FILE='C://FRAN/textdoc/in2.txt')
        read(1,*)xmin,xmax,dx,ymin,ymax,dy
        close(1)
        y=ymin
        x=xmin
        open(2,FILE='C://FRAN/textdoc/out2.txt')
4       format (G12.4, $) 
3       format (G12.4)
6       format (G24.4, $)
        write(2, 6)x
        x=x+dx
100     if(x.gt.xmax.or.x.eq.xmax)goto 5
        write(2, 4)x
        x=x+dx
        goto 100
5       write(2, 3)xmax
        x=xmin
1       if(y.gt.ymax.or.y.eq.ymax)goto 10
        write(2, 4)y
2       if(x.gt.xmax.or.x.eq.xmax)goto 20
        if((sin(x*3.14/180+cos(y*3.14/180)).eq.0)then
        write(2, 4)'div by zero'
        else 
        f=1/(sin(x*3.14/180)+cos(y*3.14/180))
        write(2, 4)f
        end if
        x=x+dx
        goto 2
20      if((sin(xmax*3.14/180)+cos(y*3.14/180)).eq.0)then
        write(2, 3)'div by zero'
        else 
        f=1/(sin(xmax*3.14/180)+cos(y*3.14/180))
        write(2, 3)f
        end if
        y=y+dy
        x=xmin
        goto 1
10      write(2, 4)ymax
15      if(x.gt.xmax.or.x.eq.xmax)goto 30
        if((sin(x*3.14/180)+cos(ymax*3.14/180)).eq.0)then
        write(2, 4)'div by zero'
        else 
        f=1/(sin(x*3.14/180)+cos(ymax*3.14/180))
        write(2, 4)f
        end if
        x=x+dx
        goto 15
30      if((sin(xmax*3.14/180)+cos(ymax*3.14/180)).eq.0)then
        write(2, 3)'div by zero'
        else 
        f=1/(sin(xmax*3.14/180)+cos(ymax*3.14/180))
        write(2, 3)f
        end if
        
        end
        
        
