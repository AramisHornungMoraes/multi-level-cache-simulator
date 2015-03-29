#Multi-Level Cache Simulator
A multi-level cache simulator with a predefined architecture.

##Introdução:
O simulador de memória cache a seguir é projetado de acordo com uma arquitetura definida,
com uma cache multinível de duas Directed Mapped para instrução e para dados em primeiro nível
e uma Fully associative em segundo nível, em paralelo está mais três Set Associative de duas,
quatro e oito níveis.
A seguir está a ilustração que representa esta arquitetura.
![Image](http://aramishornungmoraes.github.io/style/arquitetura.png)
De acordo com o determinado durante o desenvolvimento do projeto, as caches possuem
tamanhos diferentes, mas possuem mesmo tamanho de linha de cache. A cache FA possui tamanho
duas vezes o tamanho informado pelo usuário, as DMs possuem um oitavo o tamanho informado da
cache.
Note que o tamanho de cache informado está na escala de KB – kilobytes – e está sujeito a
arredondamento de valores. Vale a pena ressaltar que este valor de tamanho de cache não pode ser
menor que 1 KB e não pode ser maior que 2 GB. Além disso, foi definido durante o planejamento
que a arquitetura deste simulador seria de 32 bits.
Interface:
A seguir será mostrado os módulos de janelas de que o simulador é composto, e o que cada
uma faz, assim como um exemplo de simulação.
A imagem a seguir mostra a janela principal do simulador. Existem 3 menus: *Cache
Settings, Console, Simulation*.
![Image](http://aramishornungmoraes.github.io/style/interface1.png)

##Cache Settings
![Image](http://aramishornungmoraes.github.io/style/exemplo2.png)
Permite que o usuário defina o tamanho da cache, a linha de cache, o
tamanho da janela (em clocks) para o algoritmo de substituição LRU,
checkboxes que definem quais caches vão usar o LRU durante a
simulação, e na parte de endereços o usuário terá a opção de gerar
endereços para uma simulação, ou simular através de um arquivo *txt*.
Depois de definidas esses parâmetros é possível fazer uma
simulação.
##Console
![Image](http://aramishornungmoraes.github.io/style/exemplo1.png)
O console permite a visualização de cada passo que ocorre na simulação, nele serão
mostrados as mensagens de erro, ocorrências de cada cache a cada informação de hit e miss em
detalhe.
É altamente recomendado fechar o console na hora de executar simulações. Simulações
com grande número de endereços com console aberto pode deixar a execução da simulação lenta, e
fazer com que demore mais do que seria com console fechado para o resultado ser gerado, devido
ao fato de a engine gráfica usada consumir um tempo para a inserção do evento no histórico e
imprimir o texto no console, por isso é recomendado o fechamento do console a partir de
simulações que usem mais de 1000 endereços.
##Simulation
![Image](http://aramishornungmoraes.github.io/style/exemplo3.png)
A janela de simulação mostra as
informações gerais da simulação e controla a
mesma. Existem três botões de navegação da
simulação, um para simular de clock à clock, uma
para ir de janela de clock à janela de clock e
finalmente um para executar todo a simulação, que
na em resumo faz a simulação até acabarem os
endereços existentes no vetor de endereços.
Também existem seis botões que mostram
as views de cada memória cache da arquitetura. A
view é uma tabela que ilustra os bits de controle e
o endereço que está armazenado em qual linha e
ou via

## Contato

Para falar com o craidor do projeto, por favor, mande uma mensagem para [Aramis Hornung Moraes](https://plus.google.com/u/0/103791949210339737668/)
![Image](https://lh3.googleusercontent.com/-gJpG_OGpwAI/AAAAAAAAAAI/AAAAAAAACw4/a0uVi1JqHtQ/s120-c/photo.jpg)