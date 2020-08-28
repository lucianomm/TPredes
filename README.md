Instruções de execução:

O programa precisa inicializar o servidor, que possui a palavra chave do 
jogo da forca "hardcoded" no programa. Uma vez inicializado, um cliente 
por vez pode conectar com o servidor, e uma vez conectado, o servidor 
irá enviar uma mensagem informando o tamanho da palavra. Formato da 
mensagem -> 
"<tipo><separador><conteudo1><separador><conteudo2><sepparador>(etc.)" O 
separador também está definido dentro da funcao "decode_message()" 
presente em common.c, que para esse caso usei "|". Uma vez sabendo o 
tamanho da palavra, o cliente imprime uma sequencia de "_" representando 
letras não descobertas ainda, a cada letra descoberta, o "_" é 
substituído pela letra. O cliente envia uma mensagem (tipo '2') com uma 
letra palpite para o servidor, que recebe a letra, busca ela na palavra 
chave, e retorna uma outra mensagem (tipo '3') com o número de 
ocorrencias, e posicoes de ocorrencias na palavra. O cliente continua 
enviando palpites até que o servidor envie uma mensagem tipo '4', 
significando a descoberta total da palavra e o final do jogo
