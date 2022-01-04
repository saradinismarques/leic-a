function Erroex_2()
Emax=0;
zmax=0;
z=0:0.01:3.49;

Eex_1 = abs(Gauss_2(z,1) - cdf('normal', z, 0, 1));

plot(z,log10(Eex_1))

hold on

xlabel('z');
ylabel('Log |E|');
title('Logaritmo do erro exato em função de z para Regra de Gauss-Lobatto com 4 pontos');

Eex_5 = abs(Gauss_2(z,5) - cdf('normal', z, 0, 1));
%plot(log10(z), log10(Eex_5))
plot(z,log10(Eex_5))

Eex_10 = abs(Gauss_2(z,10) - cdf('normal', z, 0, 1));

plot(z,log10(Eex_10))

Eex_50 = abs(Gauss_2(z,50) - cdf('normal', z, 0, 1));

plot(z,log10(Eex_50))

Eex_100 = abs(Gauss_2(z,100) - cdf('normal', z, 0, 1));

plot(z,log10(Eex_100))

legend('N=1','N=5','N=10','N=50','N=100');

hold off

for  z = 0:0.01:3.49

    Eex_10 = abs(Gauss_2(z,10) - cdf('normal', z, 0, 1));

    if Emax< Eex_10
          Emax=Eex_10;
          zmax=z;
    end

end
 
fprintf('-> %E para z=%0.2f, usando a Regra de Gauss-Lobatto com 4 pontos\n',Emax,zmax);

end
