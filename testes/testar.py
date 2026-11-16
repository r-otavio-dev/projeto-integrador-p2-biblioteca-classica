#!/usr/bin/env python3
"""Testes de caixa preta: menus reais, arquivos isolados e reinicializacoes.

Somente biblioteca padrao do Python e GCC. Nao altera os CSVs da demonstracao.
Use TEST_CFLAGS para verificacoes adicionais, por exemplo -fsanitize=undefined.
"""
import csv
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile
import unittest

RAIZ = Path(__file__).resolve().parents[1]
ARQUIVOS = ("livros.csv", "leitores.csv", "emprestimos.csv", "reservas.csv")


class Biblioteca(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.compilacao = tempfile.TemporaryDirectory(prefix="biblioteca-build-")
        cls.programa = Path(cls.compilacao.name) / ("biblioteca.exe" if os.name == "nt" else "biblioteca")
        comando = ["gcc", "-std=c11", "-Wall", "-Wextra", "-Wpedantic", "-Werror"]
        comando += shlex.split(os.environ.get("TEST_CFLAGS", ""))
        subprocess.run(comando + [str(RAIZ / "main.c"), "-o", str(cls.programa)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.compilacao.cleanup()

    def setUp(self):
        self.pasta = tempfile.TemporaryDirectory(prefix="biblioteca-dados-")
        self.addCleanup(self.pasta.cleanup)
        self.dados = Path(self.pasta.name)
        for nome in ARQUIVOS:
            shutil.copyfile(RAIZ / nome, self.dados / nome)

    def executar(self, *entradas, codigo=0):
        resultado = subprocess.run(
            [str(self.programa), "--sem-pausa"],
            input="\n".join(map(str, entradas)) + "\n", text=True,
            cwd=self.dados, capture_output=True, timeout=10,
        )
        saida = resultado.stdout + resultado.stderr
        self.assertEqual(resultado.returncode, codigo, saida)
        self.assertNotIn("runtime error:", saida)
        self.assertNotIn("AddressSanitizer", saida)
        return saida

    def linhas(self, nome):
        with (self.dados / nome).open(encoding="utf-8", newline="") as arquivo:
            return list(csv.reader(arquivo, delimiter=";"))

    def por_id(self, nome, codigo):
        return next((linha for linha in self.linhas(nome) if linha[0] == str(codigo)), None)

    def gravar(self, nome, linhas):
        with (self.dados / nome).open("w", encoding="utf-8", newline="") as arquivo:
            csv.writer(arquivo, delimiter=";", lineterminator="\n").writerows(linhas)

    def fotografia(self):
        return {nome: (self.dados / nome).read_bytes() for nome in ARQUIVOS}

    def test_01_carga_listagem_relatorios_e_matriz(self):
        saida = self.executar(1, 2, 0, 2, 2, 0, 3, 2, 7, 0, 4, 2, 5, 6, 0, 0)
        self.assertIn("10 livros, 8 leitores, 8 emprestimos e 8 reservas", saida)
        self.assertIn("Disponiveis: 6 | Emprestados: 4", saida)
        self.assertIn("40.00%", saida)
        self.assertIn("R$ 569.30 | Valor medio: R$ 56.93", saida)
        self.assertIn("Multas pendentes: R$ 4.50", saida)
        self.assertIn("aquisicoes pendentes: R$ 407.50", saida)
        for categoria, disponiveis, emprestados, total in (
            ("Romance", 1, 2, 3), ("Tecnico", 1, 1, 2),
            ("Ficcao", 2, 1, 3), ("Outras", 2, 0, 2), ("TOTAL", 6, 4, 10),
        ):
            self.assertRegex(saida, rf"{categoria}\s+{disponiveis}\s+{emprestados}\s+{total}")

    def test_02_livros_crud_reabertura_e_duplicatas(self):
        self.executar(1, 1, 21, "Capitaes da Areia", "Jorge Amado", "Romance", "39.90", 0, 0)
        self.assertEqual(self.por_id("livros.csv", 21)[1:],
                         ["Capitaes da Areia", "Jorge Amado", "Romance", "39.90", "D", "1"])
        antes = self.fotografia()
        self.assertIn("Ja existe", self.executar(1, 1, 21, 0, 0))
        self.assertEqual(antes, self.fotografia())
        saida = self.executar(1, 3, 21, "Tenda dos Milagres", "Jorge Amado", "Romance", "45.50", 5, 2, "Milagres", 0, 0)
        self.assertIn("Tenda dos Milagres", saida)
        self.assertEqual(self.por_id("livros.csv", 21)[4], "45.50")
        self.assertIn("Tenda dos Milagres", self.executar(1, 5, 1, 21, 0, 0))
        self.executar(1, 4, 21, 0, 0)
        self.assertIsNone(self.por_id("livros.csv", 21))
        self.assertIn("10 livros", self.executar(0))

    def test_03_leitores_crud_e_cache_do_emprestimo(self):
        self.executar(2, 1, 21, "Isabela Carvalho", "11988887777", "V", 0, 0)
        self.assertEqual(self.por_id("leitores.csv", 21)[4], "0.00")
        antes = self.fotografia()
        self.assertIn("Ja existe", self.executar(2, 1, 21, 0, 0))
        self.assertEqual(antes, self.fotografia())
        saida = self.executar(2, 3, 21, "Isabela Nunes", "11988886666", "P", 8, "Nunes", 0, 0)
        self.assertIn("Isabela Nunes", saida)
        self.executar(2, 4, 21, 3, 1, "Ana Clara Lima", "11987654321", "P", 0, 0)
        self.assertIsNone(self.por_id("leitores.csv", 21))
        self.assertEqual(self.por_id("emprestimos.csv", 1)[3], "Ana Clara Lima")
        self.assertIn("Ana Clara Lima", self.executar(3, 5, 2, "Clara Lima", 0, 0))

    def test_04_reservas_crud_e_busca(self):
        self.executar(4, 1, 21, "Quincas Borba", "Romance", "30.00", "A", 0, 0, 0)
        antes = self.fotografia()
        self.assertIn("ja cadastrado", self.executar(4, 1, 21, 0, 0))
        self.assertEqual(antes, self.fotografia())
        saida = self.executar(4, 3, 21, "Quincas Borba revisado", "Romance", "35.00", "M", 1, 7, "Borba", 0, 0)
        self.assertIn("Quincas Borba revisado", saida)
        self.assertEqual(self.por_id("reservas.csv", 21)[3:], ["35.00", "M", "1"])
        self.executar(4, 4, 21, 0, 0)
        self.assertIsNone(self.por_id("reservas.csv", 21))
        self.assertIn("8 reservas", self.executar(0))

    def test_05_emprestimo_valida_vinculos_e_duplicatas(self):
        for entradas, mensagem in (
            ((3, 1, 1, 0, 0), "ja cadastrado"),
            ((3, 1, 21, 999, 0, 0), "Livro inexistente"),
            ((3, 1, 21, 1, 0, 0), "Livro indisponivel"),
            ((3, 1, 21, 9, 999, 0, 0), "Leitor inexistente"),
        ):
            with self.subTest(mensagem=mensagem):
                antes = self.fotografia()
                self.assertIn(mensagem, self.executar(*entradas))
                self.assertEqual(antes, self.fotografia())

    def test_06_emprestimo_renovacao_devolucao_multa_e_remocao(self):
        self.executar(3, 1, 21, 9, 1, 7, "2.50", 0, 0)
        self.assertEqual(self.por_id("livros.csv", 9)[5], "E")
        self.assertEqual(self.por_id("emprestimos.csv", 21)[6], "A")
        self.assertIn("Historia do Brasil", self.executar(3, 5, 1, 21, 5, 2, "Historia", 0, 0))
        self.executar(3, 3, 21, 10, "3.00", 0, 0)
        self.assertEqual(self.por_id("emprestimos.csv", 21)[4:6], ["10", "3.00"])
        antes = self.fotografia()
        self.assertIn("Devolva", self.executar(3, 6, 21, 0, 0))
        self.assertEqual(antes, self.fotografia())
        saida = self.executar(3, 4, 21, 13, 0, 0)
        self.assertIn("Multa: R$ 9.00", saida)
        self.assertEqual(self.por_id("livros.csv", 9)[5], "D")
        self.assertEqual(self.por_id("leitores.csv", 1)[4], "9.00")
        self.assertEqual(self.por_id("emprestimos.csv", 21)[6], "F")
        antes = self.fotografia()
        self.assertIn("ja foi devolvido", self.executar(3, 4, 21, 0, 0))
        self.assertIn("Somente emprestimos ativos", self.executar(3, 3, 21, 0, 0))
        self.assertEqual(antes, self.fotografia())
        self.executar(3, 6, 21, 0, 0)
        self.assertIsNone(self.por_id("emprestimos.csv", 21))
        self.assertEqual(self.por_id("leitores.csv", 1)[4], "9.00")
        self.assertNotIn("[ERRO]", self.executar(0))

    def test_07_devolucao_no_prazo_e_multa_acumulada(self):
        self.assertIn("Multa: R$ 0.00", self.executar(3, 4, 1, 12, 0, 0))
        self.assertEqual(self.por_id("leitores.csv", 1)[4], "0.00")
        self.executar(3, 1, 21, 3, 5, 7, "1.50", 4, 21, 9, 0, 0)
        self.assertEqual(self.por_id("leitores.csv", 5)[4], "6.00")

    def test_08_remocao_preserva_vinculos_e_saldo(self):
        antes = self.fotografia()
        self.assertIn("vinculado", self.executar(1, 4, 3, 0, 0))
        self.assertIn("historico", self.executar(2, 4, 6, 0, 0))
        self.assertEqual(antes, self.fotografia())
        self.executar(3, 6, 6, 0, 1, 4, 4, 0, 2, 4, 6, 0, 0)
        self.assertIsNone(self.por_id("livros.csv", 4))
        self.assertIsNone(self.por_id("leitores.csv", 6))
        self.executar(3, 6, 5, 0, 0)
        self.assertIn("multas pendentes", self.executar(2, 4, 5, 0, 0))
        self.assertNotIn("[ERRO]", self.executar(0))

    def test_09_buscas_e_bubble_sort_preservam_structs(self):
        self.executar(1, 1, 22, "Titulo igual", "Autor B", "Historia", 22, 1, 21, "Titulo igual", "Autor A", "Poesia", 21, 0, 0)
        anteriores = {r[0]: r for r in self.linhas("livros.csv")}
        saida = self.executar(2, 5, 2, "Casmurro", 6, "Machado", 8, "Clara", 7, 0, 0)
        for termo in ("Dom Casmurro", "Machado de Assis", "Ana Clara Souza", "Bubble Sort"):
            self.assertIn(termo, saida)
        ordenados = self.linhas("livros.csv")
        self.assertEqual({r[0]: r for r in ordenados}, anteriores)
        self.assertEqual(ordenados, sorted(ordenados, key=lambda r: (r[1].encode(), int(r[0]))))
        self.assertNotIn("[ERRO]", self.executar(0))

    def test_16_id_maximo_e_erro_ao_abrir_para_gravar(self):
        self.executar(4, 1, 10000, 9999, "Quincas Borba", "Romance", 30, "A", 0, 0, 0)
        self.assertIsNotNone(self.por_id("reservas.csv", 9999))
        self.assertIsNone(self.por_id("reservas.csv", 10000))
        caminho = self.dados / "reservas.csv"
        caminho.chmod(0o444)
        try:
            if os.access(caminho, os.W_OK):
                self.skipTest("Usuario ainda possui permissao de escrita apos chmod")
            antes = self.fotografia()
            saida = self.executar(4, 4, 9999, codigo=1)
            self.assertIn("[ERRO]", saida)
            self.assertNotIn("Reserva removida", saida)
            self.assertEqual(antes, self.fotografia())
        finally:
            caminho.chmod(0o644)

    def test_10_entradas_invalidas_e_limite_exato(self):
        titulo = "X" * 49
        autor = "Y" * 39
        saida = self.executar("abc", -1, "9999999999999999999999", 1, 1, 0, 21,
                              "", " ;titulo", "X" * 50, titulo, autor, "Categoria",
                              "nan", "inf", -1, "1,50", "12.50", 0, 0)
        self.assertGreaterEqual(saida.count("[ERRO]"), 10)
        self.assertEqual(self.por_id("livros.csv", 21)[1:3], [titulo, autor])
        self.assertEqual(self.por_id("livros.csv", 21)[4], "12.50")
        self.assertNotIn("[ERRO]", self.executar(0))

    def test_11_registros_ausentes_nao_alteram_csv(self):
        antes = self.fotografia()
        saida = self.executar(1, 3, 999, 4, 999, 5, 1, 999, 0,
                              2, 3, 999, 4, 999, 8, "Inexistente", 0,
                              3, 3, 999, 4, 999, 6, 999, 5, 1, 999, 0,
                              4, 3, 999, 4, 999, 7, "Inexistente", 0, 0)
        self.assertIn("nao encontrado", saida)
        self.assertEqual(antes, self.fotografia())

    def test_12_limite_de_50_em_cada_modulo(self):
        for nome in ARQUIVOS:
            linhas = self.linhas(nome)
            while len(linhas) < 50:
                id_novo = str(100 + len(linhas))
                modelo = list(linhas[-1])
                modelo[0] = id_novo
                linhas.append(modelo)
            self.gravar(nome, linhas)
        antes = self.fotografia()
        saida = self.executar(1, 1, 0, 2, 1, 0, 3, 1, 0, 4, 1, 0, 0)
        self.assertIn("50 livros, 50 leitores, 50 emprestimos e 50 reservas", saida)
        self.assertGreaterEqual(saida.count("[ERRO]"), 4)
        self.assertEqual(antes, self.fotografia())

    def test_13_csv_invalido_duplicado_e_excedente_nao_sobrescreve(self):
        originais = self.fotografia()
        for nome in ARQUIVOS:
            for problema in ("duplicado", "malformado", "excedente", "cr_embutido"):
                with self.subTest(nome=nome, problema=problema):
                    for arq, conteudo in originais.items():
                        (self.dados / arq).write_bytes(conteudo)
                    linhas = self.linhas(nome)
                    if problema == "duplicado":
                        linhas.append(linhas[0])
                    elif problema == "malformado":
                        linhas[0][0] = "id_invalido"
                    elif problema == "excedente":
                        while len(linhas) <= 50:
                            nova = list(linhas[-1])
                            nova[0] = str(100 + len(linhas))
                            linhas.append(nova)
                    self.gravar(nome, linhas)
                    if problema == "cr_embutido":
                        caminho = self.dados / nome
                        caminho.write_bytes(caminho.read_bytes().replace(b"\n", b"\rLIXO\n", 1))
                    antes = self.fotografia()
                    self.assertIn("[ERRO]", self.executar(0, codigo=1))
                    self.assertEqual(antes, self.fotografia())

    def test_14_integracao_inconsistente_e_csv_ausente(self):
        originais = self.fotografia()
        for problema in ("referencia", "nome", "status", "ativo_duplicado", "ausente"):
            with self.subTest(problema=problema):
                for nome, conteudo in originais.items():
                    (self.dados / nome).write_bytes(conteudo)
                if problema == "ausente":
                    (self.dados / "reservas.csv").unlink()
                    antes = (self.dados / "livros.csv").read_bytes()
                elif problema == "status":
                    linhas = self.linhas("livros.csv")
                    linhas[0][5] = "D"
                    self.gravar("livros.csv", linhas)
                else:
                    linhas = self.linhas("emprestimos.csv")
                    if problema == "referencia": linhas[0][1] = "999"
                    if problema == "nome": linhas[0][3] = "Nome divergente"
                    if problema == "ativo_duplicado":
                        linhas.append(["21"] + linhas[0][1:])
                    self.gravar("emprestimos.csv", linhas)
                self.assertIn("[ERRO]", self.executar(0, codigo=1))
                if problema == "ausente":
                    self.assertFalse((self.dados / "reservas.csv").exists())
                    self.assertEqual(antes, (self.dados / "livros.csv").read_bytes())

    def test_15_crlf_ultima_linha_sem_quebra_e_dados_vazios(self):
        for nome in ARQUIVOS:
            caminho = self.dados / nome
            caminho.write_bytes(caminho.read_bytes().rstrip(b"\n").replace(b"\n", b"\r\n"))
        self.assertNotIn("[ERRO]", self.executar(0))
        for nome in ARQUIVOS:
            (self.dados / nome).write_text("")
        saida = self.executar(4, 5, 6, 0, 2, 7, 0, 0)
        self.assertIn("0 livros, 0 leitores, 0 emprestimos e 0 reservas", saida)
        self.assertIn("0.00%", saida)
        self.assertNotIn("[ERRO]", saida)



if __name__ == "__main__":
    unittest.main(verbosity=2)
