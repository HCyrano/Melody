#!/usr/bin/env python3
"""
Lance le moteur Melody (protocole IOStd) et envoie des commandes
    IOStd search-fixed-depth <64 cases> <X|O> <depth> <precision>

Exemples :
    python3 melody_fixed_depth.py \
        "O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X" 12 100

    # plusieurs positions (une par ligne, seuls les 66 premiers caractères sont lus ; ex. fichier .obf)
    python3 melody_fixed_depth.py -f positions.txt -d 12 -p 100
"""

import argparse
import os
import queue
import re
import subprocess
import sys
import threading
import time

# Le script est placé à la racine du dépôt Melody : l'exécutable est cherché dans build/Release/
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_ENGINE = os.path.join(SCRIPT_DIR, "build", "Release", "Melody")

POS_RE = re.compile(r"^[xXbB\*oOwW\.\-]{64} [xXbB\*oOwW]$")


def parse_board(raw_text):
    """
    Extrait systématiquement la grille de 64 caractères et le joueur,
    qu'il s'agisse d'une chaîne linéaire ou d'un bloc multi-lignes.
    """
    valid_chars = re.findall(r"[XxbB\*OowW\.-]", raw_text)
    
    if len(valid_chars) < 65:
        # Retourne le texte brut s'il est trop court, pour laisser la regex globale échouer proprement
        return raw_text.strip()
    
    board_str = "".join(valid_chars[:64])
    player_str = valid_chars[64]
    
    return f"{board_str} {player_str}"


class IOStdEngine:
    def __init__(self, path, extra_args=(), cwd=None):
        path = os.path.abspath(path)
        # Par défaut : le dossier parent de Release/ (= build/), comme _2_Harmony.command
        if cwd is None:
            cwd = os.path.dirname(os.path.dirname(path))
        self.proc = subprocess.Popen(
            [path, *extra_args],
            cwd=cwd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=None,  # laisse passer les erreurs du moteur
            text=True,
            bufsize=1,
        )
        self.q = queue.Queue()
        # Thread dédié à la lecture (évite select + BufferedReader)
        threading.Thread(target=self._reader, daemon=True).start()

    def _reader(self):
        for line in self.proc.stdout:
            self.q.put(line.rstrip("\n"))
        self.q.put(None)  # EOF

    def send(self, cmd):
        if self.proc.poll() is not None:
            sys.exit(f"Le moteur s'est arrêté (code {self.proc.returncode}) "
                     f"avant d'envoyer : {cmd}")
        self.proc.stdin.write(cmd + "\n")
        self.proc.stdin.flush()

    def read_until(self, end_re=None, first_timeout=600.0, quiet=0.5):
        """Collecte les lignes de sortie.

        - attend la 1ère ligne jusqu'à first_timeout
        - s'arrête dès qu'une ligne matche end_re (si fourni)
        - sinon s'arrête après `quiet` secondes sans nouvelle ligne
        """
        lines = []
        timeout = first_timeout
        while True:
            try:
                line = self.q.get(timeout=timeout)
            except queue.Empty:
                break
            if line is None:
                break
            lines.append(line)
            if end_re and re.search(end_re, line):
                break
            timeout = quiet
        return lines

    def init(self):
        self.send("IOStd init")
        return self.read_until(end_re=r"ready", first_timeout=10.0)

    def search_fixed_depth(self, position, depth, precision, end_re=None):
        self.send(f"IOStd search-fixed-depth {position} {depth} {precision}")
        return self.read_until(end_re=end_re)

    def close(self):
        try:
            self.send("IOStd quit")
            self.proc.wait(timeout=5)
        except Exception:
            self.proc.kill()


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-e", "--engine", default=DEFAULT_ENGINE,
                    help="exécutable Melody (défaut : <dossier du script>/build/Release/Melody)")
    ap.add_argument("position", nargs="?",
                    help='"<64 cases> <X|O>" ou bloc multi-lignes entre guillemets')
    ap.add_argument("depth_pos", nargs="?", type=int, help="profondeur")
    ap.add_argument("precision_pos", nargs="?", type=int, help="précision")
    ap.add_argument("-f", "--file", help="fichier de positions")
    ap.add_argument("-d", "--depth", type=int, help="profondeur (mode fichier)")
    ap.add_argument("-p", "--precision", type=int, help="précision (mode fichier)")
    ap.add_argument("--cwd", default=None,
                    help="répertoire de lancement (défaut : parent de Release/)")
    ap.add_argument("--engine-args", default="-mode IOStd -h 25 -t 16",
                    help='arguments passés à Melody (défaut : "-mode IOStd -h 25 -t 16")')
    ap.add_argument("--end-regex", default=r"^move\s",
                    help="regex marquant la fin de la réponse du moteur "
                         "(défaut : ligne commençant par 'move')")
    args = ap.parse_args()

    if args.file:
        depth, precision = args.depth, args.precision
        if depth is None or precision is None:
            ap.error("--depth et --precision requis avec --file")
        with open(args.file) as f:
            content = f.read()
        
        # Si le fichier contient des blocs multi-lignes séparés par des crochets fermants
        # ou des lignes simples, on peut adapter le découpage. 
        # Ici, si chaque ligne est déjà une position (standard ou lue globalement) :
        positions = [parse_board(l) for l in content.splitlines() if l.strip()]
    else:
        if args.position is None or args.depth_pos is None or args.precision_pos is None:
            ap.error("usage : \"<position>\" depth precision")
        positions = [parse_board(args.position)]
        depth, precision = args.depth_pos, args.precision_pos

    for p in positions:
        if not POS_RE.match(p):
            sys.exit(f"Position invalide après parsing : {p!r}")

    eng = IOStdEngine(args.engine, args.engine_args.split(), args.cwd)
    try:
        eng.init()
        for p in positions:
            t0 = time.time()
            out = eng.search_fixed_depth(p, depth, precision, args.end_regex)
            dt = time.time() - t0
            print(f"> {p} depth={depth} precision={precision}  ({dt:.2f}s)")
            for line in out:
                print("   ", line)
    finally:
        eng.close()


if __name__ == "__main__":
    main()
