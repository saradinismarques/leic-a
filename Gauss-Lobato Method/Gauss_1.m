function y = Gauss_1(z,N)
   
   I_1 =0.5;
        
        for n = 1:1:N
            b = (z./N)*n;
            a = (z./N)*(n-1);
            
            I_1 = I_1 + (((b-a)/2).*((1/3).*f(g(-1,a,b))+(4/3).*f(g(0,a,b))+(1/3).*f(g(1,a,b))));
        
        end
        y=I_1;
end  
