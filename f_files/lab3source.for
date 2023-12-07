        subroutine input(l,d,r,b,n)
        real l((n*n-n)/2),d(n),r((n*n-n)/2),b(n)
        integer i,n 
        integer y                                    
              
        if(n.ge.999) then
        print *, 'Not enough memory'
        read(*,*)
        stop
        endif       
        print *, 'overwrite input file? yes(1)/no(0)'
        read(*,*)y
        if(y.eq.1)call test_generator(n) 
                
        open(1,FILE='C://FRAN/textdoc/left_3.txt')   
        open(2,FILE='C://FRAN/textdoc/diag_3.txt') 
        open(3,FILE='C://FRAN/textdoc/right_3.txt') 
        open(4,FILE='C://FRAN/textdoc/vector_3.txt') 
        read(1,*)(l(i),i=1,(n*n-n)/2)
                
        read(2,*)(d(i),i=1,n)
        print *, 'test'
        read(*,*) 
        read(3,*)(r(i),i=1,(n*n-n)/2)    
        read(4,*)(b(i),i=1,n)                      
        close(1)
        close(2)
        close(3)
        close(4)
        end


        subroutine process_new(al,di,ar,b,v,ia,ja,n)
        integer diag,left,right,j,i,cl,cr,temp,ia(n+1)
        integer ja(n*n-n),k
        real s,al((n*n-n)/2),di(n),ar((n*n-n)/2),b(n),v(n)
        k=ia(n+1)-1 !kolichestvo vne diagonal elementov
        do i=1,n
        s=0
!        left=ja(ia(i)) 
        cl=ia(i+1)-ia(i)
        left=ia(i)
1       if(cl.eq.0)goto 2
        s=s+al(left+cl-1)*b(ja(ia(i)+cl-1))
        cl=cl-1
        goto 1
2       s=s+di(i)*b(i)
        temp=i+1
3       if(temp.eq.n)goto 9
        cl=ia(temp+1)-ia(temp)
        left=ia(temp)
        if(cl.eq.0)goto 8
        if(ja(ia(temp)).eq.i)s=s+al(left+cl-1)*b(temp)
        cl=cl-1
8       temp=temp+1
        goto 3
9       v(i)=s
        end do
        end
        
        

        subroutine process(l,d,r,b,v,n)
        integer diag,left,right,j,i,cl,cr,temp  
        real l((n*n-n)/2),d(n),r((n*n-n)/2),b(n),v(n)
        real s               
        right=1  !nomer tekyshego elementa pravogo treygol'nika 
        left=1  
        diag=1 !nomer tekyshego diag.element      
        i=0     !nomer tekysh'ey stroki 
        j=1     !nomer tekyshego elementa vektora      
        temp=right       
4       if(i.eq.n)goto 40
        cl=i !left elements count
        cr=n-1-i !right elements count        
        k=0
        s=0
        j=1
5       if(k.eq.cl)goto 50 
        s=s+(l(left)*b(j))
        left=left+1
        k=k+1
        j=j+1
        goto 5
50      s=s+(d(diag)*b(j))
        diag=diag+1
        j=j+1
        k=0
        dx=i+1 !rasstoyanie mejdy indeksami B pravom treygol'nike       
6       if(k.eq.cr)goto 60
        k=k+1       
        s=s+(r(right)*b(j))
        j=j+1
        right=right+dx
        dx=dx+1
        goto 6
60      temp=temp+i+2
        right=temp
        v(1+i)=s
        s=0
        i=i+1
        goto 4
40      end
          

        subroutine output(v,n)
        real v(n)
        open(4,FILE='C://FRAN/textdoc/out1_3.txt') 
        do i=1,n
        write(4,*)v(i)
        end do 
        close(4)
        end
        
        
        subroutine test_generator(n)
        integer i,n
        open (1,FILE='C://FRAN/textdoc/left_3.txt')
        open (2,FILE='C://FRAN/textdoc/diag_3.txt')
        open (3,FILE='C://FRAN/textdoc/right_3.txt')
        open (4,FILE='C://FRAN/textdoc/vector_3.txt')
        write(1,*)(i,i=1,(n*n-n)/2)
        write(2,*)(i,i=1,n)
        write(3,*)(i,i=1,(n*n-n)/2)
        write(4,*)(i,i=1,n)
90      close(1)
        close(2)
        close(3)
        close(4)
        return
        end        
         
              
        program main
        integer n,a1,a2,a3,a4,a5,a6,a7     
        real a(1000000)   
        open(3,FILE='C://FRAN/textdoc/size_3.txt')     
        read(3,*)n   
        close(3)
        a1=1  !indeks pervogo elementa levogo treygol'nika
        a2=int((n*n-n)/2)+1  !indeks pervogo elementa diag
        a3=a2+n  !indeks pervogo elementa pravogo treygol'nika
        a4=n*n+1  !indeks pervogo elementa vektora
        a5=a4+n  !indeks pervogo elementa resultiruesh'ego vektora     
        a6=a5+n !indeks nachala masiva ia
        a7=a6+n+1 !indeks nachal masiva ja
        ! call input(a,n,a1,a2,a3,a4,a5)
        call input(a,a(a2),a(a3),a(a4),a(a6),a(a7),n)
!        call process(a,a(a2),a(a3),a(a4),a(a5),a(a6),n)
        call process_new(a,a(a2),a(a3),a(a4),a(a5),a(a6),a(a7),n)
        call output(a(a5),n)
        end 
        
