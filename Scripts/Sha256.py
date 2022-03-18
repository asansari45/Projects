import struct

class Sha256:
    """
    Used to calculate a SHA256 digest given a message.
    """
    def __init__(self):
        """
        Constructor
        """
        self._msg = ''

    def Ror(self, x, n):
        return (x >> n) | ((x << (32-n)) & 0xffff_ffff)
        for i in range(n):
            if r & 1 == 1:
                r = (r >> 1) | (1 << 31)
            else:
                r = r >> 1
        return r
    
    def Calculate(self, msg):
        """
        Calculates the SHA256 digest per:  https://en.wikipedia.org/wiki/SHA-2.

        Returns:
            The SHA256 digest for the msg as a string.
        """
        self._msg = msg

        # Initialize hash values:
        # (first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):        
        h0 = 0x6a09e667
        h1 = 0xbb67ae85
        h2 = 0x3c6ef372
        h3 = 0xa54ff53a
        h4 = 0x510e527f
        h5 = 0x9b05688c
        h6 = 0x1f83d9ab
        h7 = 0x5be0cd19

        # Initialize array of round constants:
        # (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
        k = [ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
              0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
              0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
              0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
              0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
              0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
              0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
              0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 ]
        
        L = len(self._msg) * 8

        # Figure out K
        n = 1
        K = 512 * n - L - 1 - 64
        while K < 0:
            n += 1
            K = 512 * n - L - 1 - 64

        # Pad the message now with 1 and 7 bits from K
        pad = bytearray(0x80)
        K = K - 7

        # Pad 
        pad = bytearray([0x80] + [0x00] * (K//8))

        # Add L as a 64-bit number big endian format.
        pad += struct.pack('>Q', L)

        msgWithPad = self._msg + pad
        print('msg len:  {0}, pad len:  {1}, msgWithPad len:  {2}'.format(len(self._msg), len(pad), len(msgWithPad)))
        print([hex(x) for x in msgWithPad])

        chunkCount = n
        w = [0] * 64
        for chunk in range(chunkCount):

            # Copy chunk into first 16 words of w[0..15]
            for i in range(16):
                start = chunk*(512//8)+i*4
                w[i] = struct.unpack('>I', msgWithPad[start:start+4])[0]
            
            for i in range(16, 64):
                s0 = self.Ror(w[i-15],7) ^ self.Ror(w[i-15],18) ^ (w[i-15] >> 3)
                s1 = self.Ror(w[i-2],17) ^ self.Ror(w[i-2],19) ^ (w[i-2] >> 10)
                w[i] = (w[i-16] + s0 + w[i-7] + s1) & 0xffff_ffff
            
            # Initialize working variables to current hash value:
            a = h0
            b = h1
            c = h2
            d = h3
            e = h4
            f = h5
            g = h6
            h = h7

            # Compression function main loop:
            for i in range(0,64):
                S1 = self.Ror(e, 6) ^ self.Ror(e,11) ^ self.Ror(e,25)
                ch = (e & f) ^ ((~e) & g)
                temp1 = (h + S1 + ch + k[i] + w[i]) & 0xffff_ffff
                S0 = self.Ror(a,2) ^ self.Ror(a,13) ^ self.Ror(a,22)
                maj = (a & b) ^ (a & c) ^ (b & c)
                temp2 = (S0 + maj) & 0xffff_ffff
 
                h = g
                g = f
                f = e
                e = (d + temp1) & 0xffff_ffff
                d = c
                c = b
                b = a
                a = (temp1 + temp2) & 0xffff_ffff

            # Add the compressed chunk to the current hash value:
            h0 = (h0 + a) & 0xffff_ffff
            h1 = (h1 + b) & 0xffff_ffff
            h2 = (h2 + c) & 0xffff_ffff
            h3 = (h3 + d) & 0xffff_ffff
            h4 = (h4 + e) & 0xffff_ffff
            h5 = (h5 + f) & 0xffff_ffff
            h6 = (h6 + g) & 0xffff_ffff
            h7 = (h7 + h) & 0xffff_ffff

        # create the final digest
        hashes = [h0,h1,h2,h3,h4,h5,h6,h7]
        digest = []
        for h in hashes:
            p = struct.pack('>I', h)
            for i in p:
                digest.append(hex(int(i)))

        return digest


print(Sha256().Calculate(bytearray('hello world', 'utf-8')))












