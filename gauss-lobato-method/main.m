function main()
    
    fprintf('\nO majorante do erro m�ximo com N=10 �:\n\n');
    subplot(3,3,1)
    Erromax_0
    subplot(3,3,2)
    Erromax_1
    subplot(3,3,3)
    Erromax_2
    
    fprintf('\n\n');
    
    fprintf('\nO erro m�ximo cometido com N=10 �:\n\n');
    subplot(3,3,4)
    Erroex_0
    subplot(3,3,5)
    Erroex_1
    subplot(3,3,6)
    Erroex_2
    
    fprintf('\n\n');
    
    subplot(3,3,8)
    ErroexN

    tabela
end

