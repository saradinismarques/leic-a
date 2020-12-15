function Erromax_1()
Emax=0;

z = 0:0.01:3.49;

y= cp(3)*((z/10).^5).*(1/sqrt(2*pi)).*(exp(-(z.^2)./2)).*(3-6*(z.^2)+z.^4).*10;


plot(z,y)

hold on

xlabel('z');
ylabel('E');
title('Majorante do erro em função de z para Regra de Simpson');

for  z = 0:0.01:3.49

    y= cp(3)*((z/10).^5).*(1/sqrt(2*pi)).*(exp(-(z.^2)./2)).*(3-6*(z.^2)+z.^4).*10;


    if Emax> y
          Emax=y;
          zmax=z;
    end

end

fprintf('-> %E para z=%0.2f, usando a Regra de Simpson\n',Emax,zmax);

end
