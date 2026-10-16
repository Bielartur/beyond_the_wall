# Beyond The Wall

Um projeto em **C** criado para praticar e aprofundar conceitos de engenharia de software "raiz": sem agentes de IA escrevendo código por você, só você, o compilador e o debugger.

O que começou como um joguinho para a disciplina de Introdução à Programação virou um laboratório para estudar arquitetura de verdade.

## 🎯 Objetivos

- **Arquitetura**: organizar o projeto em camadas e responsabilidades bem definidas
- **Modularização**: módulos pequenos, coesos e fáceis de testar
- **Clean code**: código legível, com nomes claros e funções que fazem uma coisa só
- **Uso seguro de ponteiros**: nada de ponteiros soltos, não inicializados ou apontando para onde não deviam

## 🤖 Sobre o uso de IA

A ideia aqui é **aprender fazendo**. Por isso, a IA entra como mentora, nunca como autora do código.

### ✅ Pode usar IA para

- Sugerir nomes para variáveis e funções
- Organizar commits, separando responsabilidades
- Explicar conceitos que te ajudem a destravar uma nova funcionalidade
- Ensinar técnicas de debug (como usar o `gdb`, o `valgrind`, ler um stack trace...)

### ❌ Evite usar IA para

- Escrever funcionalidades inteiras ou trechos de código para colar no projeto
- Encontrar o bug por você. **Quem acha o bug é você**: recorra à IA só se estiver travado a ponto de desanimar, e mesmo assim peça uma pista, não a resposta

> Errar, travar e depurar faz parte do aprendizado. É justamente essa a graça do projeto.

## 🚀 Como rodar

Pré-requisitos: `gcc` e `make` (Linux, macOS ou WSL no Windows).

```bash
# clone o repositório
git clone https://github.com/Bielartur/beyond_the_wall.git
cd beyond_the_wall

# compile e jogue
make run
```

Outros comandos:

```bash
make        # só compila (gera o executável ./jogo)
./jogo      # roda o jogo já compilado
make clean  # apaga os .o e o executável
```


## 🤝 Como contribuir

Contribuições são muito bem-vindas! Você pode:

1. Abrir uma **issue** relatando bugs ou sugerindo melhorias
2. Propor novas **features**
3. Enviar um **Pull Request**

Antes de enviar um PR, verifique se o código compila sem warnings e se não há vazamentos de memória (dica: `valgrind --leak-check=full`).
