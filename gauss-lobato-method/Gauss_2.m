function y = Gauss_2(z,N)
   
I_2=0.5;
        
        for n = 1:1:N
            b = (z./N)*n;
            a = (z./N)*(n-1);
            
            I_2 = I_2 + (((b-a)/2).*((1/6).*f(g(-1,a,b))+(5/6).*f(g((-1.*sqrt(5)/5),a,b))+(5/6).*f(g((sqrt(5)/5),a,b))+(1/6).*f(g(1,a,b))));
        
        
        end
        y=I_2;
end