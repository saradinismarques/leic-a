function tabela()
fp = fopen ('Tabela De Distribuicao Normal Padrao', 'w');
 
%cabecalho
fprintf (fp, 'Tabela De Distribuicao Normal Padrao \n\n');
fprintf(fp, 'z   ||    0.00|     0.01|     0.02|     0.03|     0.04|     0.05|     0.06|     0.07|     0.08|     0.09|\n');
fprintf(fp, '--------------------------------------------------4------------------------------------------------------\n');

for z=0:0.1:3.4 
    
        fprintf(fp, '%0.1f ||',z);
    for k=0:0.01:0.09
    %escreve cada linha da tabela
        fprintf(fp, ' %0.4f | %0.4f | %0.4f | %0.4f | %0.4f | %0.4f | %0.4f | %0.4f | %0.4f | %0.4f\n\n', Gauss_2(z+k,10));
    end
    fprintf(fp,'\n');
    fprintf(fp, '--------------------------------------------------------------------------------------------------------- \n');
end
 
%fecha o ficheiro
fclose (fp);
end