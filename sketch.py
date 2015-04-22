class GameGuess(object):

    def __init__(self, word):
        self.word = word
        self.guessed = [0]*len(word)

    def guessLetter(self, letter):
        occ = [i for i, l in enumerate(self.word) if l == letter]
        for i in occ:
            self.guessed[i] = 1

    def isGuessed(self):
        return all(self.guessed)

k = GameGuess("hello,idiot")
k.guessLetter("o")
k.guessLetter("e")
k.guessLetter("h")
k.guessLetter("l")
k.guessLetter(",")
k.guessLetter("i")
k.guessLetter("d")
k.guessLetter("t")
print k.isGuessed()
        
        
