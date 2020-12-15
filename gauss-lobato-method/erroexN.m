function ErroexN()
y_0=[];
y_1=[];
y_2=[];

for N=1:1:300

EexN_0 = abs(Gauss_0(1.73,N) - cdf('normal', 1.73, 0, 1));
EexN_1 = abs(Gauss_1(1.73,N) - cdf('normal', 1.73, 0, 1));
EexN_2 = abs(Gauss_2(1.73,N) - cdf('normal', 1.73, 0, 1));

y_0=[y_0,EexN_0];
y_1=[y_1,EexN_1];
y_2=[y_2,EexN_2];

end
N=1:1:300;
plot(log10(N),log10(y_0))
hold on

xlabel('Log N');
ylabel('Log |E|');
title('Logaritmo do erro exato em função do N');

plot(log10(N),log10(y_1))

plot(log10(N),log10(y_2))

legend('Regra Trapézio','Regra Simpson','Regra GL 4 pontos');

hold off

end 

