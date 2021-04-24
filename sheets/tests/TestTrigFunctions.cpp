// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestTrigFunctions.h"

#include "TestKspreadCommon.h"

void TestTrigFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation
#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),RoundNumber(y))
#define ROUND(x) (roundf(1e10 * x) / 1e10)

// round to get at most 10-digits number
static Value RoundNumber(double f)
{
    return Value(ROUND(f));
}

// round to get at most 10-digits number
static Value RoundNumber(const Value& v)
{
    if (v.isNumber()) {
        double d = numToDouble(v.asFloat());
        if (fabs(d) < DBL_EPSILON)
            d = 0.0;
        return Value(ROUND(d));
    } else
        return v;
}

Value TestTrigFunctions::evaluate(const QString& formula)
{
    Formula f;
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

#if 0
    // this magically generates the CHECKs
    printf("  CHECK_EVAL( \"%s\",  %15g) );\n", qPrintable(formula), result.asFloat());
#endif

    return RoundNumber(result);
}

void TestTrigFunctions::testCOS()
{
    // some trivial cases
    CHECK_EVAL("COS(0)", 1);
    CHECK_EVAL("COS(PI()/2)", 0);
    CHECK_EVAL("COS(PI())", -1);
    CHECK_EVAL("COS(-PI()/2)", 0);

    // 128 points in a circle
    CHECK_EVAL("COS(   0*2*PI()/128 )", 1.000000000000000);
    CHECK_EVAL("COS(   1*2*PI()/128 )", 0.998795456205172);
    CHECK_EVAL("COS(   2*2*PI()/128 )", 0.995184726672197);
    CHECK_EVAL("COS(   3*2*PI()/128 )", 0.989176509964781);
    CHECK_EVAL("COS(   4*2*PI()/128 )", 0.980785280403230);
    CHECK_EVAL("COS(   5*2*PI()/128 )", 0.970031253194544);
    CHECK_EVAL("COS(   6*2*PI()/128 )", 0.956940335732209);
    CHECK_EVAL("COS(   7*2*PI()/128 )", 0.941544065183021);
    CHECK_EVAL("COS(   8*2*PI()/128 )", 0.923879532511287);
    CHECK_EVAL("COS(   9*2*PI()/128 )", 0.903989293123443);
    CHECK_EVAL("COS(  10*2*PI()/128 )", 0.881921264348355);
    CHECK_EVAL("COS(  11*2*PI()/128 )", 0.857728610000272);
    CHECK_EVAL("COS(  12*2*PI()/128 )", 0.831469612302545);
    CHECK_EVAL("COS(  13*2*PI()/128 )", 0.803207531480645);
    CHECK_EVAL("COS(  14*2*PI()/128 )", 0.773010453362737);
    CHECK_EVAL("COS(  15*2*PI()/128 )", 0.740951125354959);
    CHECK_EVAL("COS(  16*2*PI()/128 )", 0.707106781186548);
    CHECK_EVAL("COS(  17*2*PI()/128 )", 0.671558954847018);
    CHECK_EVAL("COS(  18*2*PI()/128 )", 0.634393284163645);
    CHECK_EVAL("COS(  19*2*PI()/128 )", 0.595699304492433);
    CHECK_EVAL("COS(  20*2*PI()/128 )", 0.555570233019602);
    CHECK_EVAL("COS(  21*2*PI()/128 )", 0.514102744193222);
    CHECK_EVAL("COS(  22*2*PI()/128 )", 0.471396736825998);
    CHECK_EVAL("COS(  23*2*PI()/128 )", 0.427555093430282);
    CHECK_EVAL("COS(  24*2*PI()/128 )", 0.382683432365090);
    CHECK_EVAL("COS(  25*2*PI()/128 )", 0.336889853392220);
    CHECK_EVAL("COS(  26*2*PI()/128 )", 0.290284677254462);
    CHECK_EVAL("COS(  27*2*PI()/128 )", 0.242980179903264);
    CHECK_EVAL("COS(  28*2*PI()/128 )", 0.195090322016128);
    CHECK_EVAL("COS(  29*2*PI()/128 )", 0.146730474455362);
    CHECK_EVAL("COS(  30*2*PI()/128 )", 0.0980171403295608);
    CHECK_EVAL("COS(  31*2*PI()/128 )", 0.0490676743274181);
    CHECK_EVAL("COS(  32*2*PI()/128 )", 0.000000000000000);
    CHECK_EVAL("COS(  33*2*PI()/128 )", -0.0490676743274180);
    CHECK_EVAL("COS(  34*2*PI()/128 )", -0.0980171403295606);
    CHECK_EVAL("COS(  35*2*PI()/128 )", -0.146730474455362);
    CHECK_EVAL("COS(  36*2*PI()/128 )", -0.195090322016128);
    CHECK_EVAL("COS(  37*2*PI()/128 )", -0.242980179903264);
    CHECK_EVAL("COS(  38*2*PI()/128 )", -0.290284677254462);
    CHECK_EVAL("COS(  39*2*PI()/128 )", -0.336889853392220);
    CHECK_EVAL("COS(  40*2*PI()/128 )", -0.382683432365090);
    CHECK_EVAL("COS(  41*2*PI()/128 )", -0.427555093430282);
    CHECK_EVAL("COS(  42*2*PI()/128 )", -0.471396736825998);
    CHECK_EVAL("COS(  43*2*PI()/128 )", -0.514102744193222);
    CHECK_EVAL("COS(  44*2*PI()/128 )", -0.555570233019602);
    CHECK_EVAL("COS(  45*2*PI()/128 )", -0.595699304492433);
    CHECK_EVAL("COS(  46*2*PI()/128 )", -0.634393284163645);
    CHECK_EVAL("COS(  47*2*PI()/128 )", -0.671558954847018);
    CHECK_EVAL("COS(  48*2*PI()/128 )", -0.707106781186547);
    CHECK_EVAL("COS(  49*2*PI()/128 )", -0.740951125354959);
    CHECK_EVAL("COS(  50*2*PI()/128 )", -0.773010453362737);
    CHECK_EVAL("COS(  51*2*PI()/128 )", -0.803207531480645);
    CHECK_EVAL("COS(  52*2*PI()/128 )", -0.831469612302545);
    CHECK_EVAL("COS(  53*2*PI()/128 )", -0.857728610000272);
    CHECK_EVAL("COS(  54*2*PI()/128 )", -0.881921264348355);
    CHECK_EVAL("COS(  55*2*PI()/128 )", -0.903989293123443);
    CHECK_EVAL("COS(  56*2*PI()/128 )", -0.923879532511287);
    CHECK_EVAL("COS(  57*2*PI()/128 )", -0.941544065183021);
    CHECK_EVAL("COS(  58*2*PI()/128 )", -0.956940335732209);
    CHECK_EVAL("COS(  59*2*PI()/128 )", -0.970031253194544);
    CHECK_EVAL("COS(  60*2*PI()/128 )", -0.980785280403230);
    CHECK_EVAL("COS(  61*2*PI()/128 )", -0.989176509964781);
    CHECK_EVAL("COS(  62*2*PI()/128 )", -0.995184726672197);
    CHECK_EVAL("COS(  63*2*PI()/128 )", -0.998795456205172);
    CHECK_EVAL("COS(  64*2*PI()/128 )", -1.000000000000000);
    CHECK_EVAL("COS(  65*2*PI()/128 )", -0.998795456205172);
    CHECK_EVAL("COS(  66*2*PI()/128 )", -0.995184726672197);
    CHECK_EVAL("COS(  67*2*PI()/128 )", -0.989176509964781);
    CHECK_EVAL("COS(  68*2*PI()/128 )", -0.980785280403230);
    CHECK_EVAL("COS(  69*2*PI()/128 )", -0.970031253194544);
    CHECK_EVAL("COS(  70*2*PI()/128 )", -0.956940335732209);
    CHECK_EVAL("COS(  71*2*PI()/128 )", -0.941544065183021);
    CHECK_EVAL("COS(  72*2*PI()/128 )", -0.923879532511287);
    CHECK_EVAL("COS(  73*2*PI()/128 )", -0.903989293123443);
    CHECK_EVAL("COS(  74*2*PI()/128 )", -0.881921264348355);
    CHECK_EVAL("COS(  75*2*PI()/128 )", -0.857728610000272);
    CHECK_EVAL("COS(  76*2*PI()/128 )", -0.831469612302545);
    CHECK_EVAL("COS(  77*2*PI()/128 )", -0.803207531480645);
    CHECK_EVAL("COS(  78*2*PI()/128 )", -0.773010453362737);
    CHECK_EVAL("COS(  79*2*PI()/128 )", -0.740951125354959);
    CHECK_EVAL("COS(  80*2*PI()/128 )", -0.707106781186548);
    CHECK_EVAL("COS(  81*2*PI()/128 )", -0.671558954847019);
    CHECK_EVAL("COS(  82*2*PI()/128 )", -0.634393284163646);
    CHECK_EVAL("COS(  83*2*PI()/128 )", -0.595699304492433);
    CHECK_EVAL("COS(  84*2*PI()/128 )", -0.555570233019602);
    CHECK_EVAL("COS(  85*2*PI()/128 )", -0.514102744193222);
    CHECK_EVAL("COS(  86*2*PI()/128 )", -0.471396736825998);
    CHECK_EVAL("COS(  87*2*PI()/128 )", -0.427555093430282);
    CHECK_EVAL("COS(  88*2*PI()/128 )", -0.382683432365090);
    CHECK_EVAL("COS(  89*2*PI()/128 )", -0.336889853392220);
    CHECK_EVAL("COS(  90*2*PI()/128 )", -0.290284677254462);
    CHECK_EVAL("COS(  91*2*PI()/128 )", -0.242980179903264);
    CHECK_EVAL("COS(  92*2*PI()/128 )", -0.195090322016129);
    CHECK_EVAL("COS(  93*2*PI()/128 )", -0.146730474455362);
    CHECK_EVAL("COS(  94*2*PI()/128 )", -0.0980171403295605);
    CHECK_EVAL("COS(  95*2*PI()/128 )", -0.0490676743274180);
    CHECK_EVAL("COS(  96*2*PI()/128 )", -0.000000000000000);
    CHECK_EVAL("COS(  97*2*PI()/128 )", 0.0490676743274177);
    CHECK_EVAL("COS(  98*2*PI()/128 )", 0.0980171403295601);
    CHECK_EVAL("COS(  99*2*PI()/128 )", 0.146730474455362);
    CHECK_EVAL("COS( 100*2*PI()/128 )", 0.195090322016128);
    CHECK_EVAL("COS( 101*2*PI()/128 )", 0.242980179903264);
    CHECK_EVAL("COS( 102*2*PI()/128 )", 0.290284677254462);
    CHECK_EVAL("COS( 103*2*PI()/128 )", 0.336889853392220);
    CHECK_EVAL("COS( 104*2*PI()/128 )", 0.382683432365090);
    CHECK_EVAL("COS( 105*2*PI()/128 )", 0.427555093430282);
    CHECK_EVAL("COS( 106*2*PI()/128 )", 0.471396736825998);
    CHECK_EVAL("COS( 107*2*PI()/128 )", 0.514102744193222);
    CHECK_EVAL("COS( 108*2*PI()/128 )", 0.555570233019602);
    CHECK_EVAL("COS( 109*2*PI()/128 )", 0.595699304492433);
    CHECK_EVAL("COS( 110*2*PI()/128 )", 0.634393284163646);
    CHECK_EVAL("COS( 111*2*PI()/128 )", 0.671558954847018);
    CHECK_EVAL("COS( 112*2*PI()/128 )", 0.707106781186547);
    CHECK_EVAL("COS( 113*2*PI()/128 )", 0.740951125354959);
    CHECK_EVAL("COS( 114*2*PI()/128 )", 0.773010453362737);
    CHECK_EVAL("COS( 115*2*PI()/128 )", 0.803207531480645);
    CHECK_EVAL("COS( 116*2*PI()/128 )", 0.831469612302545);
    CHECK_EVAL("COS( 117*2*PI()/128 )", 0.857728610000272);
    CHECK_EVAL("COS( 118*2*PI()/128 )", 0.881921264348355);
    CHECK_EVAL("COS( 119*2*PI()/128 )", 0.903989293123443);
    CHECK_EVAL("COS( 120*2*PI()/128 )", 0.923879532511287);
    CHECK_EVAL("COS( 121*2*PI()/128 )", 0.941544065183021);
    CHECK_EVAL("COS( 122*2*PI()/128 )", 0.956940335732209);
    CHECK_EVAL("COS( 123*2*PI()/128 )", 0.970031253194544);
    CHECK_EVAL("COS( 124*2*PI()/128 )", 0.980785280403230);
    CHECK_EVAL("COS( 125*2*PI()/128 )", 0.989176509964781);
    CHECK_EVAL("COS( 126*2*PI()/128 )", 0.995184726672197);
    CHECK_EVAL("COS( 127*2*PI()/128 )", 0.998795456205172);

    // Cosinus needs to be a numeric value
    CHECK_EVAL("COS(raghu)", Value::errorVALUE());
    CHECK_EVAL("COS()", Value::errorVALUE());

    CHECK_EVAL("1+COS(0.2)", Value(1.980066578));
}

// hyperbolic cosine
void TestTrigFunctions::testCOSH()
{
    CHECK_EVAL("COSH(   0*PI()/64 )", 1.00000000000000);
    CHECK_EVAL("COSH(   1*PI()/64 )", 1.00120502763102);
    CHECK_EVAL("COSH(   2*PI()/64 )", 1.00482301470726);
    CHECK_EVAL("COSH(   3*PI()/64 )", 1.01086268077751);
    CHECK_EVAL("COSH(   4*PI()/64 )", 1.01933858177076);
    CHECK_EVAL("COSH(   5*PI()/64 )", 1.03027114507681);
    CHECK_EVAL("COSH(   6*PI()/64 )", 1.04368671877737);
    CHECK_EVAL("COSH(   7*PI()/64 )", 1.05961763514644);
    CHECK_EVAL("COSH(   8*PI()/64 )", 1.07810228857284);
    CHECK_EVAL("COSH(   9*PI()/64 )", 1.09918522809284);
    CHECK_EVAL("COSH(  10*PI()/64 )", 1.12291726475574);
    CHECK_EVAL("COSH(  11*PI()/64 )", 1.14935559408141);
    CHECK_EVAL("COSH(  12*PI()/64 )", 1.17856393390454);
    CHECK_EVAL("COSH(  13*PI()/64 )", 1.21061267793823);
    CHECK_EVAL("COSH(  14*PI()/64 )", 1.24557906542667);
    CHECK_EVAL("COSH(  15*PI()/64 )", 1.28354736729603);
    CHECK_EVAL("COSH(  16*PI()/64 )", 1.32460908925201);
    CHECK_EVAL("COSH(  17*PI()/64 )", 1.36886319231368);
    CHECK_EVAL("COSH(  18*PI()/64 )", 1.41641633131500);
    CHECK_EVAL("COSH(  19*PI()/64 )", 1.46738311194883);
    CHECK_EVAL("COSH(  20*PI()/64 )", 1.52188636697305);
    CHECK_EVAL("COSH(  21*PI()/64 )", 1.58005745224421);
    CHECK_EVAL("COSH(  22*PI()/64 )", 1.64203656329247);
    CHECK_EVAL("COSH(  23*PI()/64 )", 1.70797307320055);
    CHECK_EVAL("COSH(  24*PI()/64 )", 1.77802589260111);
    CHECK_EVAL("COSH(  25*PI()/64 )", 1.85236385266018);
    CHECK_EVAL("COSH(  26*PI()/64 )", 1.93116611196955);
    CHECK_EVAL("COSH(  27*PI()/64 )", 2.01462258832895);
    CHECK_EVAL("COSH(  28*PI()/64 )", 2.10293441645836);
    CHECK_EVAL("COSH(  29*PI()/64 )", 2.19631443274388);
    CHECK_EVAL("COSH(  30*PI()/64 )", 2.29498768818512);
    CHECK_EVAL("COSH(  31*PI()/64 )", 2.39919199078058);
    CHECK_EVAL("COSH(  32*PI()/64 )", 2.50917847865806);
    CHECK_EVAL("COSH(  33*PI()/64 )", 2.62521222533141);
    CHECK_EVAL("COSH(  34*PI()/64 )", 2.74757287854239);
    CHECK_EVAL("COSH(  35*PI()/64 )", 2.87655533422713);
    CHECK_EVAL("COSH(  36*PI()/64 )", 3.01247044723167);
    CHECK_EVAL("COSH(  37*PI()/64 )", 3.15564578048928);
    CHECK_EVAL("COSH(  38*PI()/64 )", 3.30642639446529);
    CHECK_EVAL("COSH(  39*PI()/64 )", 3.46517567877181);
    CHECK_EVAL("COSH(  40*PI()/64 )", 3.63227622795684);
    CHECK_EVAL("COSH(  41*PI()/64 )", 3.80813076357823);
    CHECK_EVAL("COSH(  42*PI()/64 )", 3.99316310478491);
    CHECK_EVAL("COSH(  43*PI()/64 )", 4.18781918974444);
    CHECK_EVAL("COSH(  44*PI()/64 )", 4.39256815037867);
    CHECK_EVAL("COSH(  45*PI()/64 )", 4.60790344299758);
    CHECK_EVAL("COSH(  46*PI()/64 )", 4.83434403755624);
    CHECK_EVAL("COSH(  47*PI()/64 )", 5.07243566840111);
    CHECK_EVAL("COSH(  48*PI()/64 )", 5.32275214951996);
    CHECK_EVAL("COSH(  49*PI()/64 )", 5.58589675746527);
    CHECK_EVAL("COSH(  50*PI()/64 )", 5.86250368528411);
    CHECK_EVAL("COSH(  51*PI()/64 )", 6.15323957095837);
    CHECK_EVAL("COSH(  52*PI()/64 )", 6.45880510403919);
    CHECK_EVAL("COSH(  53*PI()/64 )", 6.77993671434747);
    CHECK_EVAL("COSH(  54*PI()/64 )", 7.11740834681045);
    CHECK_EVAL("COSH(  55*PI()/64 )", 7.47203332671172);
    CHECK_EVAL("COSH(  56*PI()/64 )", 7.84466631985014);
    CHECK_EVAL("COSH(  57*PI()/64 )", 8.23620539233163);
    CHECK_EVAL("COSH(  58*PI()/64 )", 8.64759417495814);
    CHECK_EVAL("COSH(  59*PI()/64 )", 9.07982413742996);
    CHECK_EVAL("COSH(  60*PI()/64 )", 9.53393697784256);
    CHECK_EVAL("COSH(  61*PI()/64 )", 10.0110271332365);
    CHECK_EVAL("COSH(  62*PI()/64 )", 10.5122444172514);
    CHECK_EVAL("COSH(  63*PI()/64 )", 11.0387967912398);
}

void TestTrigFunctions::testPI()
{
    CHECK_EVAL("PI()", 3.14159265358979);
    CHECK_EVAL("2*PI()", 6.28318530717959);
    CHECK_EVAL("3*PI()", 9.42477796076938);
    CHECK_EVAL("PI()/2", 1.57079632679490);
    CHECK_EVAL("PI()/PI()", 1.0);
    CHECK_EVAL("PI()/(2*PI())", 0.5);
    CHECK_EVAL("(2*PI())/(2*PI())", 1.0);
}

void TestTrigFunctions::testSIN()
{
    // some trivial cases
    CHECK_EVAL("SIN(0)", 0);
    CHECK_EVAL("SIN(PI()/2)", 1);
    CHECK_EVAL("SIN(PI())", 0);
    CHECK_EVAL("SIN(-PI()/2)", -1);

    // 128 points in a circle
    CHECK_EVAL("SIN(   0*2*PI()/128 )", 0.000000000000000);
    CHECK_EVAL("SIN(   1*2*PI()/128 )", 0.049067674327418);
    CHECK_EVAL("SIN(   2*2*PI()/128 )", 0.0980171403295606);
    CHECK_EVAL("SIN(   3*2*PI()/128 )", 0.146730474455362);
    CHECK_EVAL("SIN(   4*2*PI()/128 )", 0.195090322016128);
    CHECK_EVAL("SIN(   5*2*PI()/128 )", 0.242980179903264);
    CHECK_EVAL("SIN(   6*2*PI()/128 )", 0.290284677254462);
    CHECK_EVAL("SIN(   7*2*PI()/128 )", 0.336889853392220);
    CHECK_EVAL("SIN(   8*2*PI()/128 )", 0.382683432365090);
    CHECK_EVAL("SIN(   9*2*PI()/128 )", 0.427555093430282);
    CHECK_EVAL("SIN(  10*2*PI()/128 )", 0.471396736825998);
    CHECK_EVAL("SIN(  11*2*PI()/128 )", 0.514102744193222);
    CHECK_EVAL("SIN(  12*2*PI()/128 )", 0.555570233019602);
    CHECK_EVAL("SIN(  13*2*PI()/128 )", 0.595699304492433);
    CHECK_EVAL("SIN(  14*2*PI()/128 )", 0.634393284163645);
    CHECK_EVAL("SIN(  15*2*PI()/128 )", 0.671558954847018);
    CHECK_EVAL("SIN(  16*2*PI()/128 )", 0.707106781186547);
    CHECK_EVAL("SIN(  17*2*PI()/128 )", 0.740951125354959);
    CHECK_EVAL("SIN(  18*2*PI()/128 )", 0.773010453362737);
    CHECK_EVAL("SIN(  19*2*PI()/128 )", 0.803207531480645);
    CHECK_EVAL("SIN(  20*2*PI()/128 )", 0.831469612302545);
    CHECK_EVAL("SIN(  21*2*PI()/128 )", 0.857728610000272);
    CHECK_EVAL("SIN(  22*2*PI()/128 )", 0.881921264348355);
    CHECK_EVAL("SIN(  23*2*PI()/128 )", 0.903989293123443);
    CHECK_EVAL("SIN(  24*2*PI()/128 )", 0.923879532511287);
    CHECK_EVAL("SIN(  25*2*PI()/128 )", 0.941544065183021);
    CHECK_EVAL("SIN(  26*2*PI()/128 )", 0.956940335732209);
    CHECK_EVAL("SIN(  27*2*PI()/128 )", 0.970031253194544);
    CHECK_EVAL("SIN(  28*2*PI()/128 )", 0.980785280403230);
    CHECK_EVAL("SIN(  29*2*PI()/128 )", 0.989176509964781);
    CHECK_EVAL("SIN(  30*2*PI()/128 )", 0.995184726672197);
    CHECK_EVAL("SIN(  31*2*PI()/128 )", 0.998795456205172);
    CHECK_EVAL("SIN(  32*2*PI()/128 )", 1.000000000000000);
    CHECK_EVAL("SIN(  33*2*PI()/128 )", 0.998795456205172);
    CHECK_EVAL("SIN(  34*2*PI()/128 )", 0.995184726672197);
    CHECK_EVAL("SIN(  35*2*PI()/128 )", 0.989176509964781);
    CHECK_EVAL("SIN(  36*2*PI()/128 )", 0.980785280403230);
    CHECK_EVAL("SIN(  37*2*PI()/128 )", 0.970031253194544);
    CHECK_EVAL("SIN(  38*2*PI()/128 )", 0.956940335732209);
    CHECK_EVAL("SIN(  39*2*PI()/128 )", 0.941544065183021);
    CHECK_EVAL("SIN(  40*2*PI()/128 )", 0.923879532511287);
    CHECK_EVAL("SIN(  41*2*PI()/128 )", 0.903989293123443);
    CHECK_EVAL("SIN(  42*2*PI()/128 )", 0.881921264348355);
    CHECK_EVAL("SIN(  43*2*PI()/128 )", 0.857728610000272);
    CHECK_EVAL("SIN(  44*2*PI()/128 )", 0.831469612302545);
    CHECK_EVAL("SIN(  45*2*PI()/128 )", 0.803207531480645);
    CHECK_EVAL("SIN(  46*2*PI()/128 )", 0.773010453362737);
    CHECK_EVAL("SIN(  47*2*PI()/128 )", 0.740951125354959);
    CHECK_EVAL("SIN(  48*2*PI()/128 )", 0.707106781186548);
    CHECK_EVAL("SIN(  49*2*PI()/128 )", 0.671558954847019);
    CHECK_EVAL("SIN(  50*2*PI()/128 )", 0.634393284163645);
    CHECK_EVAL("SIN(  51*2*PI()/128 )", 0.595699304492433);
    CHECK_EVAL("SIN(  52*2*PI()/128 )", 0.555570233019602);
    CHECK_EVAL("SIN(  53*2*PI()/128 )", 0.514102744193222);
    CHECK_EVAL("SIN(  54*2*PI()/128 )", 0.471396736825998);
    CHECK_EVAL("SIN(  55*2*PI()/128 )", 0.427555093430282);
    CHECK_EVAL("SIN(  56*2*PI()/128 )", 0.382683432365090);
    CHECK_EVAL("SIN(  57*2*PI()/128 )", 0.336889853392220);
    CHECK_EVAL("SIN(  58*2*PI()/128 )", 0.290284677254462);
    CHECK_EVAL("SIN(  59*2*PI()/128 )", 0.242980179903264);
    CHECK_EVAL("SIN(  60*2*PI()/128 )", 0.195090322016129);
    CHECK_EVAL("SIN(  61*2*PI()/128 )", 0.146730474455362);
    CHECK_EVAL("SIN(  62*2*PI()/128 )", 0.0980171403295608);
    CHECK_EVAL("SIN(  63*2*PI()/128 )", 0.049067674327418);
    CHECK_EVAL("SIN(  64*2*PI()/128 )", 0.000000000000000);
    CHECK_EVAL("SIN(  65*2*PI()/128 )", -0.0490676743274177);
    CHECK_EVAL("SIN(  66*2*PI()/128 )", -0.0980171403295606);
    CHECK_EVAL("SIN(  67*2*PI()/128 )", -0.146730474455362);
    CHECK_EVAL("SIN(  68*2*PI()/128 )", -0.195090322016128);
    CHECK_EVAL("SIN(  69*2*PI()/128 )", -0.242980179903264);
    CHECK_EVAL("SIN(  70*2*PI()/128 )", -0.290284677254462);
    CHECK_EVAL("SIN(  71*2*PI()/128 )", -0.336889853392220);
    CHECK_EVAL("SIN(  72*2*PI()/128 )", -0.382683432365090);
    CHECK_EVAL("SIN(  73*2*PI()/128 )", -0.427555093430282);
    CHECK_EVAL("SIN(  74*2*PI()/128 )", -0.471396736825998);
    CHECK_EVAL("SIN(  75*2*PI()/128 )", -0.514102744193222);
    CHECK_EVAL("SIN(  76*2*PI()/128 )", -0.555570233019602);
    CHECK_EVAL("SIN(  77*2*PI()/128 )", -0.595699304492433);
    CHECK_EVAL("SIN(  78*2*PI()/128 )", -0.634393284163645);
    CHECK_EVAL("SIN(  79*2*PI()/128 )", -0.671558954847018);
    CHECK_EVAL("SIN(  80*2*PI()/128 )", -0.707106781186547);
    CHECK_EVAL("SIN(  81*2*PI()/128 )", -0.740951125354959);
    CHECK_EVAL("SIN(  82*2*PI()/128 )", -0.773010453362737);
    CHECK_EVAL("SIN(  83*2*PI()/128 )", -0.803207531480645);
    CHECK_EVAL("SIN(  84*2*PI()/128 )", -0.831469612302545);
    CHECK_EVAL("SIN(  85*2*PI()/128 )", -0.857728610000272);
    CHECK_EVAL("SIN(  86*2*PI()/128 )", -0.881921264348355);
    CHECK_EVAL("SIN(  87*2*PI()/128 )", -0.903989293123443);
    CHECK_EVAL("SIN(  88*2*PI()/128 )", -0.923879532511287);
    CHECK_EVAL("SIN(  89*2*PI()/128 )", -0.941544065183021);
    CHECK_EVAL("SIN(  90*2*PI()/128 )", -0.956940335732209);
    CHECK_EVAL("SIN(  91*2*PI()/128 )", -0.970031253194544);
    CHECK_EVAL("SIN(  92*2*PI()/128 )", -0.980785280403230);
    CHECK_EVAL("SIN(  93*2*PI()/128 )", -0.989176509964781);
    CHECK_EVAL("SIN(  94*2*PI()/128 )", -0.995184726672197);
    CHECK_EVAL("SIN(  95*2*PI()/128 )", -0.998795456205172);
    CHECK_EVAL("SIN(  96*2*PI()/128 )", -1.000000000000000);
    CHECK_EVAL("SIN(  97*2*PI()/128 )", -0.998795456205172);
    CHECK_EVAL("SIN(  98*2*PI()/128 )", -0.995184726672197);
    CHECK_EVAL("SIN(  99*2*PI()/128 )", -0.989176509964781);
    CHECK_EVAL("SIN( 100*2*PI()/128 )", -0.980785280403230);
    CHECK_EVAL("SIN( 101*2*PI()/128 )", -0.970031253194544);
    CHECK_EVAL("SIN( 102*2*PI()/128 )", -0.956940335732209);
    CHECK_EVAL("SIN( 103*2*PI()/128 )", -0.941544065183021);
    CHECK_EVAL("SIN( 104*2*PI()/128 )", -0.923879532511287);
    CHECK_EVAL("SIN( 105*2*PI()/128 )", -0.903989293123443);
    CHECK_EVAL("SIN( 106*2*PI()/128 )", -0.881921264348355);
    CHECK_EVAL("SIN( 107*2*PI()/128 )", -0.857728610000272);
    CHECK_EVAL("SIN( 108*2*PI()/128 )", -0.831469612302545);
    CHECK_EVAL("SIN( 109*2*PI()/128 )", -0.803207531480645);
    CHECK_EVAL("SIN( 110*2*PI()/128 )", -0.773010453362737);
    CHECK_EVAL("SIN( 111*2*PI()/128 )", -0.740951125354959);
    CHECK_EVAL("SIN( 112*2*PI()/128 )", -0.707106781186548);
    CHECK_EVAL("SIN( 113*2*PI()/128 )", -0.671558954847019);
    CHECK_EVAL("SIN( 114*2*PI()/128 )", -0.634393284163646);
    CHECK_EVAL("SIN( 115*2*PI()/128 )", -0.595699304492433);
    CHECK_EVAL("SIN( 116*2*PI()/128 )", -0.555570233019602);
    CHECK_EVAL("SIN( 117*2*PI()/128 )", -0.514102744193222);
    CHECK_EVAL("SIN( 118*2*PI()/128 )", -0.471396736825998);
    CHECK_EVAL("SIN( 119*2*PI()/128 )", -0.427555093430283);
    CHECK_EVAL("SIN( 120*2*PI()/128 )", -0.382683432365090);
    CHECK_EVAL("SIN( 121*2*PI()/128 )", -0.336889853392220);
    CHECK_EVAL("SIN( 122*2*PI()/128 )", -0.290284677254462);
    CHECK_EVAL("SIN( 123*2*PI()/128 )", -0.242980179903264);
    CHECK_EVAL("SIN( 124*2*PI()/128 )", -0.195090322016129);
    CHECK_EVAL("SIN( 125*2*PI()/128 )", -0.146730474455362);
    CHECK_EVAL("SIN( 126*2*PI()/128 )", -0.0980171403295605);
    CHECK_EVAL("SIN( 127*2*PI()/128 )", -0.0490676743274181);
}

// hyperbolic sine
void TestTrigFunctions::testSINH()
{
    CHECK_EVAL("SINH(   0*PI()/64 )", 0.00000000000000);
    CHECK_EVAL("SINH(   1*PI()/64 )", 0.0491071008473137);
    CHECK_EVAL("SINH(   2*PI()/64 )", 0.0983325525214279);
    CHECK_EVAL("SINH(   3*PI()/64 )", 0.147794991081176);
    CHECK_EVAL("SINH(   4*PI()/64 )", 0.197613623736882);
    CHECK_EVAL("SINH(   5*PI()/64 )", 0.247908516146325);
    CHECK_EVAL("SINH(   6*PI()/64 )", 0.298800881779610);
    CHECK_EVAL("SINH(   7*PI()/64 )", 0.350413374050329);
    CHECK_EVAL("SINH(   8*PI()/64 )", 0.402870381917066);
    CHECK_EVAL("SINH(   9*PI()/64 )", 0.456298329667662);
    CHECK_EVAL("SINH(  10*PI()/64 )", 0.510825981608731);
    CHECK_EVAL("SINH(  11*PI()/64 )", 0.566584752394762);
    CHECK_EVAL("SINH(  12*PI()/64 )", 0.623709023744691);
    CHECK_EVAL("SINH(  13*PI()/64 )", 0.682336468309276);
    CHECK_EVAL("SINH(  14*PI()/64 )", 0.742608381469790);
    CHECK_EVAL("SINH(  15*PI()/64 )", 0.804670021867697);
    CHECK_EVAL("SINH(  16*PI()/64 )", 0.868670961486010);
    CHECK_EVAL("SINH(  17*PI()/64 )", 0.934765446126030);
    CHECK_EVAL("SINH(  18*PI()/64 )", 1.00311276714826);
    CHECK_EVAL("SINH(  19*PI()/64 )", 1.07387764537337);
    CHECK_EVAL("SINH(  20*PI()/64 )", 1.14723062806849);
    CHECK_EVAL("SINH(  21*PI()/64 )", 1.22334849997556);
    CHECK_EVAL("SINH(  22*PI()/64 )", 1.30241470937230);
    CHECK_EVAL("SINH(  23*PI()/64 )", 1.38461981019272);
    CHECK_EVAL("SINH(  24*PI()/64 )", 1.47016192127261);
    CHECK_EVAL("SINH(  25*PI()/64 )", 1.55924720382691);
    CHECK_EVAL("SINH(  26*PI()/64 )", 1.65209035830962);
    CHECK_EVAL("SINH(  27*PI()/64 )", 1.74891514185372);
    CHECK_EVAL("SINH(  28*PI()/64 )", 1.84995490753831);
    CHECK_EVAL("SINH(  29*PI()/64 )", 1.95545316678234);
    CHECK_EVAL("SINH(  30*PI()/64 )", 2.06566417622064);
    CHECK_EVAL("SINH(  31*PI()/64 )", 2.18085355047644);
    CHECK_EVAL("SINH(  32*PI()/64 )", 2.30129890230729);
    CHECK_EVAL("SINH(  33*PI()/64 )", 2.42729051166718);
    CHECK_EVAL("SINH(  34*PI()/64 )", 2.55913202529720);
    CHECK_EVAL("SINH(  35*PI()/64 )", 2.69714118853103);
    CHECK_EVAL("SINH(  36*PI()/64 )", 2.84165061107874);
    CHECK_EVAL("SINH(  37*PI()/64 )", 2.99300856863454);
    CHECK_EVAL("SINH(  38*PI()/64 )", 3.15157984224051);
    CHECK_EVAL("SINH(  39*PI()/64 )", 3.31774659742900);
    CHECK_EVAL("SINH(  40*PI()/64 )", 3.49190930526272);
    CHECK_EVAL("SINH(  41*PI()/64 )", 3.67448770749215);
    CHECK_EVAL("SINH(  42*PI()/64 )", 3.86592182815631);
    CHECK_EVAL("SINH(  43*PI()/64 )", 4.06667303406504);
    CHECK_EVAL("SINH(  44*PI()/64 )", 4.27722514671850);
    CHECK_EVAL("SINH(  45*PI()/64 )", 4.49808560834373);
    CHECK_EVAL("SINH(  46*PI()/64 )", 4.72978670485843);
    CHECK_EVAL("SINH(  47*PI()/64 )", 4.97288684870950);
    CHECK_EVAL("SINH(  48*PI()/64 )", 5.22797192467780);
    CHECK_EVAL("SINH(  49*PI()/64 )", 5.49565670189296);
    CHECK_EVAL("SINH(  50*PI()/64 )", 5.77658631546086);
    CHECK_EVAL("SINH(  51*PI()/64 )", 6.07143782127495);
    CHECK_EVAL("SINH(  52*PI()/64 )", 6.38092182775833);
    CHECK_EVAL("SINH(  53*PI()/64 )", 6.70578420846934);
    CHECK_EVAL("SINH(  54*PI()/64 )", 7.04680789969806);
    CHECK_EVAL("SINH(  55*PI()/64 )", 7.40481478738601);
    CHECK_EVAL("SINH(  56*PI()/64 )", 7.78066768791671);
    CHECK_EVAL("SINH(  57*PI()/64 )", 8.17527242755082);
    CHECK_EVAL("SINH(  58*PI()/64 )", 8.58958002551754);
    CHECK_EVAL("SINH(  59*PI()/64 )", 9.02458898602345);
    CHECK_EVAL("SINH(  60*PI()/64 )", 9.48134770470283);
    CHECK_EVAL("SINH(  61*PI()/64 )", 9.96095699530914);
    CHECK_EVAL("SINH(  62*PI()/64 )", 10.4645727427369);
    CHECK_EVAL("SINH(  63*PI()/64 )", 10.9934086887683);
}

void TestTrigFunctions::testTAN()
{
    // some trivial cases
    CHECK_EVAL("TAN(0)", 0);
    CHECK_EVAL("TAN(PI()/4)", 1);
    CHECK_EVAL("TAN(PI())", 0);
    CHECK_EVAL("TAN(-PI()/4)", -1);

    // 128 points in a circle, except where the result is infinity
    CHECK_EVAL("TAN(   0*2*PI()/128 )", 0.000000000000000);
    CHECK_EVAL("TAN(   1*2*PI()/128 )", 0.0491268497694673);
    CHECK_EVAL("TAN(   2*2*PI()/128 )", 0.0984914033571642);
    CHECK_EVAL("TAN(   3*2*PI()/128 )", 0.148335987538347);
    CHECK_EVAL("TAN(   4*2*PI()/128 )", 0.198912367379658);
    CHECK_EVAL("TAN(   5*2*PI()/128 )", 0.250486960191305);
    CHECK_EVAL("TAN(   6*2*PI()/128 )", 0.303346683607342);
    CHECK_EVAL("TAN(   7*2*PI()/128 )", 0.357805721314524);
    CHECK_EVAL("TAN(   8*2*PI()/128 )", 0.414213562373095);
    CHECK_EVAL("TAN(   9*2*PI()/128 )", 0.472964775891320);
    CHECK_EVAL("TAN(  10*2*PI()/128 )", 0.534511135950792);
    CHECK_EVAL("TAN(  11*2*PI()/128 )", 0.599376933681924);
    CHECK_EVAL("TAN(  12*2*PI()/128 )", 0.668178637919299);
    CHECK_EVAL("TAN(  13*2*PI()/128 )", 0.741650546272035);
    CHECK_EVAL("TAN(  14*2*PI()/128 )", 0.820678790828660);
    CHECK_EVAL("TAN(  15*2*PI()/128 )", 0.906347169019147);
    CHECK_EVAL("TAN(  16*2*PI()/128 )", 1.000000000000000);
    CHECK_EVAL("TAN(  17*2*PI()/128 )", 1.103329975733476);
    CHECK_EVAL("TAN(  18*2*PI()/128 )", 1.218503525587976);
    CHECK_EVAL("TAN(  19*2*PI()/128 )", 1.348343913486720);
    CHECK_EVAL("TAN(  20*2*PI()/128 )", 1.496605762665489);
    CHECK_EVAL("TAN(  21*2*PI()/128 )", 1.668399205583507);
    CHECK_EVAL("TAN(  22*2*PI()/128 )", 1.870868411789389);
    CHECK_EVAL("TAN(  23*2*PI()/128 )", 2.114322357548640);
    CHECK_EVAL("TAN(  24*2*PI()/128 )", 2.414213562373095);
    CHECK_EVAL("TAN(  25*2*PI()/128 )", 2.794812772490477);
    CHECK_EVAL("TAN(  26*2*PI()/128 )", 3.296558208938321);
    CHECK_EVAL("TAN(  27*2*PI()/128 )", 3.992223783770083);
    CHECK_EVAL("TAN(  28*2*PI()/128 )", 5.027339492125846);
    CHECK_EVAL("TAN(  29*2*PI()/128 )", 6.741452405414988);
    CHECK_EVAL("TAN(  30*2*PI()/128 )", 10.153170387608842);
    CHECK_EVAL("TAN(  31*2*PI()/128 )", 20.355467624987138);
    CHECK_EVAL("TAN(  33*2*PI()/128 )", -20.355467624987192);
    CHECK_EVAL("TAN(  34*2*PI()/128 )", -10.153170387608855);
    CHECK_EVAL("TAN(  35*2*PI()/128 )", -6.741452405414994);
    CHECK_EVAL("TAN(  36*2*PI()/128 )", -5.027339492125850);
    CHECK_EVAL("TAN(  37*2*PI()/128 )", -3.992223783770084);
    CHECK_EVAL("TAN(  38*2*PI()/128 )", -3.296558208938323);
    CHECK_EVAL("TAN(  39*2*PI()/128 )", -2.794812772490478);
    CHECK_EVAL("TAN(  40*2*PI()/128 )", -2.4142135623731);
    CHECK_EVAL("TAN(  41*2*PI()/128 )", -2.114322357548642);
    CHECK_EVAL("TAN(  42*2*PI()/128 )", -1.870868411789389);
    CHECK_EVAL("TAN(  43*2*PI()/128 )", -1.668399205583508);
    CHECK_EVAL("TAN(  44*2*PI()/128 )", -1.496605762665490);
    CHECK_EVAL("TAN(  45*2*PI()/128 )", -1.348343913486720);
    CHECK_EVAL("TAN(  46*2*PI()/128 )", -1.218503525587977);
    CHECK_EVAL("TAN(  47*2*PI()/128 )", -1.103329975733476);
    CHECK_EVAL("TAN(  48*2*PI()/128 )", -1.000000000000000);
    CHECK_EVAL("TAN(  49*2*PI()/128 )", -0.906347169019148);
    CHECK_EVAL("TAN(  50*2*PI()/128 )", -0.820678790828660);
    CHECK_EVAL("TAN(  51*2*PI()/128 )", -0.741650546272036);
    CHECK_EVAL("TAN(  52*2*PI()/128 )", -0.668178637919299);
    CHECK_EVAL("TAN(  53*2*PI()/128 )", -0.599376933681924);
    CHECK_EVAL("TAN(  54*2*PI()/128 )", -0.534511135950792);
    CHECK_EVAL("TAN(  55*2*PI()/128 )", -0.472964775891320);
    CHECK_EVAL("TAN(  56*2*PI()/128 )", -0.414213562373095);
    CHECK_EVAL("TAN(  57*2*PI()/128 )", -0.357805721314524);
    CHECK_EVAL("TAN(  58*2*PI()/128 )", -0.303346683607342);
    CHECK_EVAL("TAN(  59*2*PI()/128 )", -0.250486960191306);
    CHECK_EVAL("TAN(  60*2*PI()/128 )", -0.198912367379658);
    CHECK_EVAL("TAN(  61*2*PI()/128 )", -0.148335987538347);
    CHECK_EVAL("TAN(  62*2*PI()/128 )", -0.0984914033571645);
    CHECK_EVAL("TAN(  63*2*PI()/128 )", -0.0491268497694672);
    CHECK_EVAL("TAN(  64*2*PI()/128 )", -0.000000000000000);
    CHECK_EVAL("TAN(  65*2*PI()/128 )", 0.049126849769467);
    CHECK_EVAL("TAN(  66*2*PI()/128 )", 0.0984914033571642);
    CHECK_EVAL("TAN(  67*2*PI()/128 )", 0.148335987538347);
    CHECK_EVAL("TAN(  68*2*PI()/128 )", 0.198912367379658);
    CHECK_EVAL("TAN(  69*2*PI()/128 )", 0.250486960191305);
    CHECK_EVAL("TAN(  70*2*PI()/128 )", 0.303346683607342);
    CHECK_EVAL("TAN(  71*2*PI()/128 )", 0.357805721314524);
    CHECK_EVAL("TAN(  72*2*PI()/128 )", 0.414213562373095);
    CHECK_EVAL("TAN(  73*2*PI()/128 )", 0.472964775891320);
    CHECK_EVAL("TAN(  74*2*PI()/128 )", 0.534511135950792);
    CHECK_EVAL("TAN(  75*2*PI()/128 )", 0.599376933681923);
    CHECK_EVAL("TAN(  76*2*PI()/128 )", 0.668178637919298);
    CHECK_EVAL("TAN(  77*2*PI()/128 )", 0.741650546272035);
    CHECK_EVAL("TAN(  78*2*PI()/128 )", 0.820678790828660);
    CHECK_EVAL("TAN(  79*2*PI()/128 )", 0.906347169019147);
    CHECK_EVAL("TAN(  80*2*PI()/128 )", 1.000000000000000);
    CHECK_EVAL("TAN(  81*2*PI()/128 )", 1.103329975733475);
    CHECK_EVAL("TAN(  82*2*PI()/128 )", 1.218503525587975);
    CHECK_EVAL("TAN(  83*2*PI()/128 )", 1.348343913486721);
    CHECK_EVAL("TAN(  84*2*PI()/128 )", 1.496605762665489);
    CHECK_EVAL("TAN(  85*2*PI()/128 )", 1.668399205583507);
    CHECK_EVAL("TAN(  86*2*PI()/128 )", 1.870868411789388);
    CHECK_EVAL("TAN(  87*2*PI()/128 )", 2.114322357548638);
    CHECK_EVAL("TAN(  88*2*PI()/128 )", 2.414213562373091);
    CHECK_EVAL("TAN(  89*2*PI()/128 )", 2.794812772490478);
    CHECK_EVAL("TAN(  90*2*PI()/128 )", 3.296558208938320);
    CHECK_EVAL("TAN(  91*2*PI()/128 )", 3.992223783770080);
    CHECK_EVAL("TAN(  92*2*PI()/128 )", 5.027339492125837);
    CHECK_EVAL("TAN(  93*2*PI()/128 )", 6.741452405414962);
    CHECK_EVAL("TAN(  94*2*PI()/128 )", 10.1531703876089);
    CHECK_EVAL("TAN(  95*2*PI()/128 )", 20.3554676249872);
    CHECK_EVAL("TAN(  97*2*PI()/128 )", -20.3554676249873);
    CHECK_EVAL("TAN(  98*2*PI()/128 )", -10.1531703876089);
    CHECK_EVAL("TAN(  99*2*PI()/128 )", -6.741452405414980);
    CHECK_EVAL("TAN( 100*2*PI()/128 )", -5.027339492125847);
    CHECK_EVAL("TAN( 101*2*PI()/128 )", -3.992223783770086);
    CHECK_EVAL("TAN( 102*2*PI()/128 )", -3.296558208938324);
    CHECK_EVAL("TAN( 103*2*PI()/128 )", -2.794812772490481);
    CHECK_EVAL("TAN( 104*2*PI()/128 )", -2.414213562373094);
    CHECK_EVAL("TAN( 105*2*PI()/128 )", -2.114322357548640);
    CHECK_EVAL("TAN( 106*2*PI()/128 )", -1.870868411789390);
    CHECK_EVAL("TAN( 107*2*PI()/128 )", -1.668399205583508);
    CHECK_EVAL("TAN( 108*2*PI()/128 )", -1.496605762665490);
    CHECK_EVAL("TAN( 109*2*PI()/128 )", -1.348343913486722);
    CHECK_EVAL("TAN( 110*2*PI()/128 )", -1.218503525587976);
    CHECK_EVAL("TAN( 111*2*PI()/128 )", -1.103329975733476);
    CHECK_EVAL("TAN( 112*2*PI()/128 )", -1.000000000000000);
    CHECK_EVAL("TAN( 113*2*PI()/128 )", -0.906347169019148);
    CHECK_EVAL("TAN( 114*2*PI()/128 )", -0.820678790828661);
    CHECK_EVAL("TAN( 115*2*PI()/128 )", -0.741650546272035);
    CHECK_EVAL("TAN( 116*2*PI()/128 )", -0.668178637919299);
    CHECK_EVAL("TAN( 117*2*PI()/128 )", -0.599376933681924);
    CHECK_EVAL("TAN( 118*2*PI()/128 )", -0.534511135950792);
    CHECK_EVAL("TAN( 119*2*PI()/128 )", -0.472964775891321);
    CHECK_EVAL("TAN( 120*2*PI()/128 )", -0.414213562373096);
    CHECK_EVAL("TAN( 121*2*PI()/128 )", -0.357805721314524);
    CHECK_EVAL("TAN( 122*2*PI()/128 )", -0.303346683607343);
    CHECK_EVAL("TAN( 123*2*PI()/128 )", -0.250486960191306);
    CHECK_EVAL("TAN( 124*2*PI()/128 )", -0.198912367379658);
    CHECK_EVAL("TAN( 125*2*PI()/128 )", -0.148335987538348);
    CHECK_EVAL("TAN( 126*2*PI()/128 )", -0.0984914033571642);
    CHECK_EVAL("TAN( 127*2*PI()/128 )", -0.0491268497694673);
}

void TestTrigFunctions::testCSC()
{
    CHECK_EVAL("CSC(1)", 1.1883951057781212);
}

void TestTrigFunctions::testCSCH()
{
    CHECK_EVAL("CSCH(1)", 0.8509181282393215);
}

void TestTrigFunctions::testSEC()
{
    CHECK_EVAL("SEC(1)", 1.850815717680925617);
}

void TestTrigFunctions::testSECH()
{
    CHECK_EVAL("SECH(1)", 0.6480542736638853996);
}

QTEST_MAIN(TestTrigFunctions)
