/*******************************************************************************
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 *
*******************************************************************************/

#ifndef K_REM_PIO2_C__ /*/////////////////////////////////////////////////////*/
#define K_REM_PIO2_C__

#ifdef COMPILER_HAS_PRAGMA_ONCE
#pragma once
#endif

/*////////////////////////////////////////////////////////////////////////////*/

/* -------------------------------------------------------------------------- */

#define zero   0.0
#define one    1.0
#define two24  1.67772160000000000000e+07 /* 0x41700000, 0x00000000 */
#define twon24 5.96046447753906250000e-08 /* 0x3E700000, 0x00000000 */

/* -------------------------------------------------------------------------- */

GLOBAL const int init_jk[] = { 2,3,4,6 };
GLOBAL const double PIo2[]
{
    1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
    7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
    5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
    3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
    1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
    1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
    2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
    2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
};

/* -------------------------------------------------------------------------- */

STDDEF int __kernel_rem_pio2 (double* x, double* y, int e0, int nx, int prec, const int32_t* ipio2)
{
    int32_t jz,jx,jv,jp,jk,carry,n,iq[20],i,j,k,m,q0,ih;
    double z,fw,f[20],fq[20],q[20];

    jk = init_jk[prec];
    jp = jk;

    jx =  nx-1;
    jv = (e0-3)/24; if(jv<0) jv=0;
    q0 =  e0-24*(jv+1);

    j = jv-jx; m = jx+jk;
    for (i=0; i<=m; i++,j++) f[i] = (j<0) ? zero : (double) ipio2[j];

    for (i=0; i<=jk; i++)
    {
        for (j=0,fw=0.0; j<=jx; j++) fw += x[j]*f[jx+i-j];
        q[i] = fw;
    }

    jz = jk;

recompute:
    for (i=0,j=jz,z=q[jz]; j>0; i++,j--)
    {
        fw    =  (double)((int32_t)(twon24* z));
        iq[i] =  (int32_t)(z-two24*fw);
        z     =  q[j-1]+fw;
    }

    z  = impl__scalbn(z,q0);
    z -= 8.0*impl__floor(z*0.125);
    n  = (int32_t)z;
    z -= (double)n;
    ih = 0;
    if (q0>0)
    {
        i  = (iq[jz-1]>>(24-q0)); n += i;
        iq[jz-1] -= i<<(24-q0);
        ih = iq[jz-1]>>(23-q0);
    }
    else if (q0==0)
    {
        ih = iq[jz-1]>>23;
    }
    else if (z>=0.5)
    {
        ih=2;
    }

    if (ih>0)
    {
        n += 1; carry = 0;
        for (i=0; i<jz ;i++)
        {
            j = iq[i];
            if (carry==0)
            {
                if (j!=0)
                {
                    carry = 1; iq[i] = 0x1000000- j;
                }
            }
            else
            {
                iq[i] = 0xffffff - j;
            }
        }
        if (q0>0)
        {
            switch (q0)
            {
            case 1: iq[jz-1] &= 0x7fffff; break;
            case 2: iq[jz-1] &= 0x3fffff; break;
            }
        }
        if (ih==2)
        {
            z = one - z;
            if (carry!=0) z -= impl__scalbn(one,q0);
        }
    }

    if (z==zero)
    {
        j = 0;
        for (i=jz-1; i>=jk; i--) j |= iq[i];
        if (j==0)
        {
            for (k=1; iq[jk-k]==0;k ++);
            for (i=jz+1; i<=jz+k; i++)
            {
                f[jx+i] = (double) ipio2[jv+i];
                for (j=0,fw=0.0; j<=jx; j++) fw += x[j]*f[jx+i-j];
                q[i] = fw;
            }
            jz += k;
            goto recompute;
        }
    }

    if (z==0.0)
    {
        jz -= 1; q0 -= 24;
        while (iq[jz]==0)
        {
            jz--;
            q0-=24;
        }
    }
    else
    {
        z = impl__scalbn(z,-q0);
        if (z>=two24)
        {
            fw = (double)((int32_t)(twon24*z));
            iq[jz] = (int32_t)(z-two24*fw);
            jz += 1; q0 += 24;
            iq[jz] = (int32_t) fw;
        }
        else
        {
            iq[jz] = (int32_t)z;
        }
    }

    fw = impl__scalbn(one,q0);
    for (i=jz; i>=0; i--)
    {
        q[i] = fw*(double)iq[i]; fw*=twon24;
    }

    for (i=jz; i>=0; i--)
    {
        for (fw=0.0,k=0; k<=jp&&k<=jz-i; k++) fw += PIo2[k]*q[i+k];
        fq[jz-i] = fw;
    }

    switch(prec)
    {
        case 0:
        {
            fw = 0.0;
            for (i=jz; i>=0; i--) fw += fq[i];
            y[0] = (ih==0)? fw: -fw;
        } break;
        case 1:
        case 2:
        {
            fw = 0.0;
            for (i=jz; i>=0; i--) fw += fq[i];
            y[0] = (ih==0)? fw: -fw;
            fw = fq[0]-fw;
            for (i=1; i<=jz; i++) fw += fq[i];
            y[1] = (ih==0)? fw: -fw;
        } break;
        case 3:
        {
            for (i=jz; i>0; i--)
            {
                fw      = fq[i-1]+fq[i];
                fq[i]  += fq[i-1]-fw;
                fq[i-1] = fw;
            }
            for (i=jz; i>1; i--)
            {
                fw      = fq[i-1]+fq[i];
                fq[i]  += fq[i-1]-fw;
                fq[i-1] = fw;
            }
            for (fw=0.0,i=jz; i>=2; i--) fw += fq[i];
            if(ih==0)
            {
                y[0] =  fq[0];
                y[1] =  fq[1];
                y[2] =  fw;
            }
            else
            {
                y[0] = -fq[0];
                y[1] = -fq[1];
                y[2] = -fw;
            }
        } break;
    }

    return n&7;
}

/* -------------------------------------------------------------------------- */

#undef zero
#undef one
#undef two24
#undef twon24

/* -------------------------------------------------------------------------- */

/*////////////////////////////////////////////////////////////////////////////*/

#endif /* K_REM_PIO2_C__ /////////////////////////////////////////////////////*/
