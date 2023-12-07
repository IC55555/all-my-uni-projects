        program main
        implicit double precision (a-h, o-z)
        common /partition/ lines
        common /interval/ a, b, step 
    
        dimension memory(100000000) ! 100 000 000
        
        call readdata()
        call makepartition(memory(1))
        
        result_2 = gaussianquadrature_3(memory(1))
        result_5 = gaussianquadrature_1(memory(1))
        result_6 = simpson(memory(1))
                
        print *, 'gaussianquadrature_1: ', result_5
        print *, 'gaussianquadrature_3: ', result_2
        print *, 'simpson: ', result_6
        print *, ' '
        pause

        end
      
******functions_definition
        double precision function fun(x)
        implicit double precision (a-h, o-z)
        fun = x**7
        end
      
        subroutine readdata()
        implicit double precision (a-h, o-z)
        common /partition/ lines
        common /interval/ a, b, step 
        
        open(1, file='C://FRAN/textdoc/input4_1.txt',err = 1)
        read(1, *) a, b, lines
        close(1)
        
        if (lines .eq. 0.0) goto 3
        
        step = (b - a) / lines
      
        print *, 'input data reading success.'
        return  
1       pause 'input error: "input.txt" file does not exist!'
        stop 
3       pause 'input error: zero steps!'
        stop
        end
        
        subroutine makepartition(partition)
        implicit double precision (a-h, o-z)
        common /partition/ lines
        common /interval/ a, b, step 
        
        dimension partition(*)
        
        do i = 0, lines
          partition(i + 1) = a + i * step
        end do
        if (partition(lines) .ne. b) then
          lines = lines + 1
          partition(lines) = b
        end if
        
        print *, 'partition created.'
        return  
        end
        
        
        double precision function gaussianquadrature_1(partition)
        implicit double precision (a-h, o-z)
        common /partition/ lines
        common /interval/ a, b, step 
        
        dimension partition(*)
        
        x=0.0
        w=2.0    
  
        sum = 0.0
        do i = 1, lines - 1
          shift=partition(i + 1) + partition(i)
          
          value=(x * step + shift) / 2
          
          tmp_sum = (w*fun(value))
           sum = sum + tmp_sum
        end do
       
        sum = sum * step / 2

        gaussianquadrature_1 = sum
        end
        
        
        double precision function gaussianquadrature_3(partition)
        implicit double precision (a-h, o-z)
!        real x0,x1,x2,w0,w1,w2
        common /partition/ lines
        common /interval/ a, b, step 
        
        dimension partition(*)
        
        
        x0 = -sqrt(3.0)/sqrt(5.0)
        x1 = 0.0 
        x2 = -x0 
        
        w0 = 0.555555556
        w1 = 0.888888889
        w2 = 0.555555556
  
        sum = 0.0
        do i = 1, lines - 1
          shift = partition(i + 1) + partition(i)
          
          value_0 = (x0 * step + shift) / 2
          value_1 = (x1 * step + shift) / 2
          value_2 = (x2 * step + shift) / 2
          
          
          tmp_sum=(w0*fun(value_0)+w1*fun(value_1)+w2*fun(value_2))
          
           sum = sum + tmp_sum
        end do
       
        sum = sum * step / 2

        gaussianquadrature_3 = sum
        end
        
        
        
        double precision function simpson(partition)
        implicit double precision (a-h, o-z)
        integer n
        
        common /partition/ lines
        common /interval/ a, b, step 
!        dimension partition(*)        
        n=lines 
        if(mod(n,2).eq.1)n=n+1
        dx=(b-a)/n
!        print *, dx
!        read(*,*)
        x=a 
        sum=fun(x)
        
        do i = 1, n-1
        x=a+i*dx
          tmp_sum=(fun(x))
          if(mod(i,2).eq.1)then
          tmp_sum=tmp_sum*4
          else
          tmp_sum=tmp_sum*2
          end if
          
          sum=sum+tmp_sum
        end do
        sum=sum+fun(b)
        sum=sum*dx/3

        simpson = sum
        end

