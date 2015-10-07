FileName = login
CC = gcc
pgFalgs = -I /usr/include/postgresql -lpq
CFlags = `pkg-config --cflags --libs gtk+-3.0`



all:$(FileName)

$(FileName):$(FileName).o
	$(CC) $(FileName).o -o $(FileName) $(CFlags) $(pgFalgs)
	@echo "O Arquivo $(FileName) está Pronto..."

$(FileName).o:$(FileName).c
	$(CC) -c $(FileName).c $(CFlags) $(pgFalgs)
	@echo "O Arquivo $(FileName).o está Pronto..."

clean:
	rm $(FileName) $(FileName).o

run:$(FileName)
	./$(FileName)
help:
	@echo "Ajuda de Compilação do Arquivo $(FileName):"
	@echo "-----------------------Parametros-----------------------------"
	@echo "all ou Nada: criar um arquivo $(FileName).o e $(FileName)"
	@echo "clean: Limpar os arquivos criados $(FileName).o e $(FileName)"
	@echo "help: Emitir está mensagem..."
	@echo "--------------------------------------------------------------"
	@echo "Antes de Compilar troque o valor das Variaveis do Arquivo"
	@echo "         $(FileName).c"
	@echo "Essas Variaveis estão dentro do Metodo - conexao."
