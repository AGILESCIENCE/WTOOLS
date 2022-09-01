import numpy as np

# Get he wavenumber in cycles per unit distance (both temporal or spatial)
def wavenumber(n,dx=1.):

    # Positive freq.
    kp = np.fix(n / 2.)
    fp = np.arange(1., kp + 1) / (n * dx)

    # Negative freq., obtained reversing the positive freq.
    kn = np.fix((n - 1.) / 2.)
    idn = np.arange(kn - 1, -1, -1, dtype="int32")

    # Frequency
    w = 2.*np.pi*np.concatenate(([0.], fp, -fp[idn]))

    return w


# Get next power of two
def next_power_of_two(n):
    return np.power(2, np.ceil(np.log2(n))).astype(int)


# Pad a vector to the next power of two
def pad_vector(x):
    n = len(x)
    m = next_power_of_two(n)
    if m > n:
        m = m - n
        return np.concatenate((x, np.zeros(m, dtype=x.dtype)))
    else:
        return x


def pad_matrix(m):
    r = m.shape[0]
    rp = next_power_of_two(r)
    c = m.shape[1]
    cp = next_power_of_two(c)
    if cp > c:
        t = np.zeros((r, cp - c), dtype=m.dtype)
        m = np.concatenate((m, t), axis=1)
    if rp > r:
        t = np.zeros((rp - r, cp), dtype=m.dtype)
        m = np.concatenate((m, t), axis=0)
    return m


# Dilate a function with L2 normalization: use a to dilate on time/space domain then use 1/a to dilate the FT
def dilate1d(g, x, a):
    return 1. / np.sqrt(a) * g(x / a)


def dilate2d(g, x, y, a):
    return 1. / a * g(x / a, y / a)


# CWT
def cwt1d(psi, a, g, d=1., pad=True):

    gmean = np.mean(g)
    g = g - gmean

    # Pad to the next power of 2 and compute the FFT
    if pad:
        ghat = np.fft.fft(pad_vector(g))
    else:
        ghat = np.fft.fft(g)

    # Compute the wavenumbers
    w = wavenumber(len(ghat), d)

    # Compute CWT
    gcwt = np.zeros((len(a), len(g)))
    for i in range(0, len(a)):

        # FFT of the dilated wavelet
        psihat = dilate1d(psi.signalft, w, 1. / a[i])  # / d

        # Convolution
        wt = np.fft.ifft(psihat * ghat)

        # Get rid of padding and keep only the real part
        if pad:
            gcwt[i] = np.real(wt[0:len(g)])  # * d
        else:
            gcwt[i] = np.real(wt)  # * d

    return gcwt, gmean


def icwt1d(psi, a, gcwt, d=1., pad=True):

    if pad:
        pad_len = next_power_of_two(gcwt.shape[1])
    else:
        pad_len = gcwt.shape[1]

    # Compute the wavenumbers
    w = wavenumber(pad_len, d)

    # Compute the inverse cwt coefficients
    icoef = np.zeros((len(a), gcwt.shape[1]))
    for i in range(0, len(a)):

        # FFT of the dilated wavelet
        psihat = dilate1d(psi.signalft, w, 1. / a[i])  # / d

        # FFT of the CWT coeffs at a given scale
        if pad:
            gcwthat = np.fft.fft(pad_vector(gcwt[i]))
        else:
            gcwthat = np.fft.fft(gcwt[i])

        # Convolution
        wt = np.fft.ifft(psihat * gcwthat)

        # Get rid of padding and keep only the real part
        if pad:
            icoef[i] = np.real(wt[0:gcwt.shape[1]])  # * d
        else:
            icoef[i] = np.real(wt)

    return icoef


def cwt2d(psi, a, g, d=1., pad=True):

    gmean = np.mean(g)
    g = g - gmean

    # Pad to the next power of 2 and compute the FFT
    if pad:
        ghat = np.fft.fft2(pad_matrix(g))
    else:
        ghat = np.fft.fft2(g)

    # Compute the wavenumbers
    urng = wavenumber(ghat.shape[1], d)
    vrng = wavenumber(ghat.shape[0], d)
    u, v = np.meshgrid(urng, vrng)

    # Compute CWT
    gcwt = np.zeros((len(a), g.shape[0], g.shape[1]))
    for i in range(0, len(a)):

        # FFT of the dilated wavelet
        psihat = dilate2d(psi.signalft, u, v,  1. / a[i])  # / d

        # Convolution
        wt = np.fft.ifft2(psihat * ghat)

        # Get rid of padding and keep only the real part
        if pad:
            gcwt[i] = np.real(wt[0:g.shape[0],0:g.shape[1]])  # * d
        else:
            gcwt[i] = np.real(wt)  # * d

    return gcwt, gmean


def icwt2d(psi, a, gcwt, d=1., pad=True):

    if pad:
        pad_len = next_power_of_two(gcwt.shape[1:])
    else:
        pad_len = gcwt.shape[1:]

    # Compute the wavenumbers
    urng = wavenumber(pad_len[1], d)
    vrng = wavenumber(pad_len[0], d)
    u, v = np.meshgrid(urng, vrng)

    # Compute the inverse cwt coefficients
    icoef = np.zeros((len(a), gcwt.shape[1], gcwt.shape[2]))
    for i in range(0, len(a)):

        # FFT of the dilated wavelet
        psihat = dilate2d(psi.signalft, u, v, 1. / a[i])  # / d

        # FFT of the CWT coeffs at a given scale
        if pad:
            gcwthat = np.fft.fft2(pad_matrix(gcwt[i]))
        else:
            gcwthat = np.fft.fft2(gcwt[i])

        # Convolution
        wt = np.fft.ifft2(psihat * gcwthat)

        # Get rid of padding and keep only the real part
        if pad:
            icoef[i] = np.real(wt[0:gcwt.shape[1],0:gcwt.shape[2]])  # * d
        else:
            icoef[i] = np.real(wt)

    return icoef


# Mexican hat / Ricker / Marr wavelet with sig = 1.
class MexicanHat1d:

    # Bandwidth
    WIDTH = 2. * 0.88521

    # Normalization factor to have L2 norm = 1
    NORM = 2. / (np.sqrt(3.) * np.float_power(np.pi, 1. / 4.))

    # Admissibility condition constant (used in the resolution of the identity and the inverse)
    C = np.pi * np.square(NORM)

    def __init__(self):
        pass

    def signal(self,x):
        x2 = np.square(x)
        return (1. - x2) * np.exp(-x2 / 2.) * self.NORM

    def signalft(self,w):
        w2 = np.square(w)
        return np.sqrt(2. * np.pi) * w2 * np.exp(-w2 / 2.) * self.NORM


class MexicanHat2d:

    # Bandwidth
    WIDTH = 2. * 0.88521

    # Normalization factor to have L2 norm = 1
    NORM = np.sqrt(2./np.pi)

    # Admissibility condition constant (used in the resolution of the identity and the inverse)
    C = np.float_power(np.pi, 3.) * np.square(NORM)

    def __init__(self):
        pass

    def signal(self, x, y):
        d2 = (np.square(x) + np.square(y)) / 2.
        return (1. - d2) * np.exp(-d2) * self.NORM

    def signalft(self, wx, wy):
        w2 = (np.square(wx) + np.square(wy)) / 2.
        return 2. * np.pi * w2 * np.exp(-w2) * self.NORM

# Get interclass entropy from probability density
def get_interclass_entropy(p):

    e = np.zeros_like(p)

    pi = p[0]

    for i in range(0,len(p)):

        # Class 1
        e1 = 0
        for j in range(0,i):
            if p[j] > 0:
                x = p[j]/pi
                e1 -= x*np.log2(x)

        # Class 2
        e2 = 0
        for j in range(i,len(p)):
            if p[j] > 0:
                x = p[j]/(1 - pi)
                e2 -= x*np.log2(x)

        pi += p[i]

        e[i] = e1 + e2

    return e

def met2d(gcwt,n):

    mask = np.zeros_like(gcwt, dtype='b')
    vmin = np.zeros(gcwt.shape[0])
    vmax = np.zeros(gcwt.shape[0])
    th = np.zeros(gcwt.shape[0])

    for i in range(0, gcwt.shape[0]):

        vmin[i] = np.amin(gcwt[i])
        vmax[i] = np.amax(gcwt[i])

        pdf, bin_edges = np.histogram(gcwt[i], bins=n, density=False)

        # Keep the left edge
        bin_edges = bin_edges[0:-1]

        # Remove zero pdf to avoid division by 0, not needed
        #sel_id = pdf > 0
        #pdf = pdf[sel_id]
        #bin_edges = bin_edges[sel_id]

        # Correct normalization, underestimate threshold
        # bin_width = bin_edges[1] - bin_edges[0]
        # area = np.sum(pdf)*bin_width

        # Naive normalization, TODO: why this works?
        area = np.sum(pdf)
        pdf = pdf / area

        # Get the threshold that maximize the interclass entropy
        ent = get_interclass_entropy(pdf)
        th[i] = bin_edges[np.nanargmax(ent)]

        mask[i][gcwt[i]>th[i]] = 1

    return vmin, vmax, th, mask