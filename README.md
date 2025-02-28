# memorizacao_dinamica_de_traces_com_reuso_de_valores_de_instrucoes_de_acesso_a_memoria

MEMORIZAÇÃO DINÂMICA DE TRACES COM REUSO DE VALORES DE INSTRUÇÕES DE ACESSO À MEMÓRIA

AUTOR: Luiz Marcio Faria de Aquino Viana

TESE SUBMETIDA AO CORPO DOCENTE DA COORDENAÇÃO DOS PROGRAMAS DE PÓS-GRADUAÇÃO DE ENGENHARIA DA UNIVERSIDADE FEDERAL DO RIO DE JANEIRO COMO PARTE DOS REQUISITOS NECESSÁRIOS PARA A OBTENÇÃO DO GRAU DE MESTRE EM CIÊNCIAS EM ENGENHARIA DE SISTEMAS DE COMPUTAÇÃO.

APROVADA POR:

  Prof. Felipe Maia Galvão França, Ph.D.
  Prof. Amarildo Teodoro da Costa, D.Sc.
  Prof. Philippe Olivier Alexandre Navaux, Dr. Ing.
  Profa. Anna Dolejsi Santos, D.Sc.

# Dedicatória

DEDICATÓRIA

À Mila, minha esposa, e aos meus filhos, Luiz Felipe e Maria Julia, por toda a alegria e força que me proporcionam.

# Agradecimentos

AGRADECIMENTOS

Agredeço a Deus pela força e luz recebida durante o desenvolvimento deste trabalho e pela oportunidade que eu tive em ampliar o meu conhecimento.

Aos meus orientadores, Eliseu Monteiro Chaves Filho e Felipe Maia Galvão França pelos valiosos conhecimentos recebidos e pela orientação dedicada e profissional que foi fundamental para o amadurecimento e finalização deste trabalho.

Agradeço em especial ao apoio recebido do amigo Amarildo Teodoro da Costa pelas diversas conversas e idéias que trocamos durante o desenvolvimento desta pesquisa e que foram fundamentais para a compreenção do mecanismo base deste trabalho.

À todos os companheiros de trabalho da COPPE/Sistemas que de forma direta ou indireta contribuiram com o desenvolvimento deste trabalho e em especial aos companheiros de turma Álvaro da Silva Ferreira, Magnos Martinelli, Élcio Pinecchi, Alexis Braga Kropotoff e Igor Briglia Habib de Almeida Alves pelo incentivo mútuo na conquista de nossos objetivos.

À minha esposa, Mila, e aos meus filhos, Luiz Felipe e Maria Julia, pelo apoio, carinho e compreenção recebidos.

Aos meus pais, José Luiz de Aquino Viana e Maria Cristina Faria de Aquino Viana, pelo apoio e constante incentivo recebidos durante toda a minha vida.

# Resumo

Resumo da Tese apresentada à COPPE/UFRJ como parte dos requisitos necessários para a obtenção do grau de Mestre em Ciências (M.Sc.)

MEMORIZAÇÃO DINÂMICA DE TRACES COM REUSO DE VALORES DE INSTRUÇÕES DE ACESSO À MEMÓRIA

Luiz Marcio Faria de Aquino Viana
Março/2002

Orientadores: Eliseu Chaves Filho e Felipe França Galvão.

Programa: Engenharia de Sistemas e Computação

No passado, com o objetivo de aumentar o desempenho dos processadores, as novas tecnologias focavam a otimização da velocidade dos processadores sem a devida avaliação da quantidade de trabalho redundante executado. Recentemente, novos mecanismos foram introduzidos com o intuito de prover desempenho através da redução da quantidade de trabalho redundante executado por um processador e, entre estes, as técnicas de reuso dinâmico de traces vem se destacando bastante. Este trabalho introduz o novo mecanismo de reuso dinâmico de traces denominado Dynamic Trace Memoization with Reuse of Memory Values, DTMm, que estende as funcionalidades do mecanismo original, DTM (Dynamic Trace Memoization), com a adição de reuso de valores de instruções de acesso à memória. Os experimentos realizados com o DTMm apresentaram um crescimento médio de 4.7% na aceleração sobre o mecanismo original e 7.9% sobre a arquitetura base para um benchmark composto por um subconjunto de programas do SPECint' 95.

# Abstract

Abstract of Thesis presented to COPPE/UFRJ as a partial fulfillment of the requeriments for degree of Master of Science (M.Sc.)

DYNAMIC TRACE MEMOIZATION WITH REUSE OF MEMORY ACCESS INSTRUCTIONS' VALUES

Luiz Marcio Faria de Aquino Viana
Março/2002

Advisors: Eliseu Chaves Filho and Felipe França Galvão.

Department: Engenharia de Sistemas e Computação

In the past, with the objective to improve the speed of processors, the new technologies were looking for optimizations in processors speed without taking profit from redundant work. Recently, new mecanisms were introduced with the intention to improve speed with the reduction in the quantities of redundant work, and dynamic trace reuse techniques have been introduced. This work introduce a new scheme for trace reuse named Dynamic Trace Memoization with Reuse of Memory Values, DTMm, which extend the funtionality of DTM (Dynamic Trace Memoization), by adding reuse of values of memory access instructions. Experiments done with DTMm shows a growth of 4.7% in speed over the original mecanism and 7.9% over the superscalar base architecture for a benchmark based on a subset of programs from SPECint '95.

#
