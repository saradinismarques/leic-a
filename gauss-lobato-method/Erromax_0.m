function Erromax_0()
Emax=0;

z = 0:0.01:3.49;

y= cp(2)*((z/10).^3).*((-1/sqrt(2*pi))*exp(-(z.^2)./2)+(1/sqrt(2*pi))*z.^2.*exp(-(z.^2)./2))*10;

plot(z,y)

hold on

xlabel('z');
ylabel('E');
title('Majorante do erro em função de z para Regra do Trapézio');

for  z = 0:0.01:3.49

    y = cp(2)*((z/10).^3).*((-1/sqrt(2*pi))*exp(-(z.^2)./2)+(1/sqrt(2*pi))*z.^2.*exp(-(z.^2)./2))*10;

    if Emax> y
          Emax=y;
          zmax=z;
    end

end

fprintf('-> %E para z=%0.2f, usando a Regra do Trapézio\n',Emax,zmax);

end