import sys
import os

def generate_image_file(N, filename):
    """
    Gera um arquivo de texto NxN com um quadrado branco no centro.
    """
    try:
        N = int(N)
        if N <= 0:
            raise ValueError("N must be a positive integer.")
            
        print(f"Gerando arquivo '{filename}' de tamanho {N}x{N}...")
        
        # Define o tamanho do quadrado branco central (aprox. 1/3 da imagem)
        start = N // 3
        end = N - (N // 3)

        with open(filename, 'w') as f:
            for i in range(N):
                line_data = []
                for j in range(N):
                    # Se (i, j) estiver dentro do quadrado central
                    if start <= i < end and start <= j < end:
                        line_data.append("255")
                    else:
                        line_data.append("0")
                
                # Escreve a linha no arquivo
                f.write(" ".join(line_data) + "\n")
                
                # Imprime o progresso
                if (i + 1) % (N // 10 if N >= 10 else 1) == 0:
                    print(f"  ...{(i+1)*100/N:.0f}% completo")

        print(f"Arquivo '{filename}' gerado com sucesso!")
        print(f"Tamanho do arquivo: {os.path.getsize(filename) / (1024*1024):.2f} MB")

    except ValueError as e:
        print(f"Erro: {e}", file=sys.stderr)
        print("Uso: python generate_image.py <N> <nome_do_arquivo>", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Um erro inesperado ocorreu: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Uso: python generate_image.py <N> <nome_do_arquivo>", file=sys.stderr)
        print("Exemplo: python generate_image.py 500 imagem_500.txt")
        sys.exit(1)
        
    generate_image_file(sys.argv[1], sys.argv[2])