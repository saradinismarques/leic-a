function Erromax_2()
Emax=0;

z = 0:0.01:3.49;

y= cp(4)*((z/10).^7).*(1/sqrt(2*pi)).*(exp(-(z.^2)./2)).*(-15+45*(z.^2)-15*z.^4+z.^6).*10;


plot(z,y)

xlabel('z');
ylabel('E');
title('Majorante do erro em função de z para Regra de Gauss-Lobatto com 4 pontos');

for  z = 0:0.01:3.49

     y= cp(4)*((z/10).^7).*(1/sqrt(2*pi)).*(exp(-(z.^2)./2)).*(-15+45*(z.^2)-15*z.^4+z.^6).*10;


    if Emax < y
          Emax=y;
          zmax=z;
    end

end

fprintf('-> %E para z=%0.2f, usando a Regra de Gauss-Lobatto com 4 pontos\n',Emax,zmax);

end
