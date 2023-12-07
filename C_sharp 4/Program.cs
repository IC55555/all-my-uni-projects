using System;
using System.Collections.Generic;
using System.Linq;

namespace Lab_С_sharp_3
{
    interface IExpr
    {
        double Compute(IReadOnlyDictionary<string, double> variableValues);
        IEnumerable<string> Variables { get; }
        bool IsConstant { get; }
        bool IsPolynom { get; }
    }

    public abstract class Expr : IExpr
    {
        public abstract IEnumerable<string> Variables { get; }

        public abstract bool IsConstant { get; }

        public abstract bool IsPolynom { get; }

        public static Expr operator /(Expr l, Expr r) => new Divide(l, r);
        public static Expr operator -(Expr l, Expr r) => new Sub(l, r);
        public static Expr operator *(Expr l, Expr r) => new Mult(l, r);
        public static Expr operator +(Expr l, Expr r) => new Add(l, r);
        public static Expr operator -(Expr e) => new Mult(-1, e);
        public static implicit operator Expr(double v) => new Constant(v);
        public static implicit operator Expr(string v) => new Variable(v);

        public abstract double Compute(IReadOnlyDictionary<string, double> variableValues);
    }

    public abstract class UnaryOperation : Expr
    {
        public override IEnumerable<string> Variables => _operand1.Variables;
        public UnaryOperation(Expr operand1)
        {
            _operand1 = operand1;
        }
        protected Expr _operand1;
    }

    public abstract class Function : UnaryOperation
    {
        public Function(Expr e) : base(e) { }
    }

    public class Variable : Expr
    {
        public override IEnumerable<string> Variables => new List<string> { name };
        public override bool IsConstant => false;
        public override bool IsPolynom => true;
        private string name;
        //public double value;
        public Variable(string Name)
        {
            name = Name;
            //value = 0;
        }
        //public static Variable operator +(Variable A, Variable B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value + B.value;
        //    return C;
        //}
        //public static Variable operator -(Variable A, Variable B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value - B.value;
        //    return C;
        //}
        //public static Variable operator *(Variable A, Variable B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value * B.value;
        //    return C;
        //}
        //public static Variable operator /(Variable A, Variable B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value / B.value;
        //    return C;
        //}

        //public static Variable operator +(Variable A, Constant B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value + B.value;
        //    return C;
        //}
        //public static Variable operator -(Variable A, Constant B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value - B.value;
        //    return C;
        //}
        //public static Variable operator *(Variable A, Constant B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value * B.value;
        //    return C;
        //}
        //public static Variable operator /(Variable A, Constant B)
        //{
        //    Variable C = new Variable("c");
        //    C.value = A.value / B.value;
        //    return C;
        //}
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            if (!variableValues.TryGetValue(name, out var value))
                throw new Exception($"Не введена переменная: {value}");
            return value;
        }
        public override string ToString()
        {
            return name;
        }
    }

    public abstract class BinaryOperation : Expr
    {
        public override IEnumerable<string> Variables => _operand1.Variables.Union(_operand2.Variables);
        //public string Name;
        public Variable X { get; set; }
        public Variable Y { get; set; }
        Variable Result { get; }
        //public BinaryOperation(string Name)
        //{
        //    name = Name;
        //    value = 0;
        //}
        public BinaryOperation(Expr operand1, Expr operand2)
        {
            _operand1 = operand1;
            _operand2 = operand2;
        }
        protected Expr _operand1;
        protected Expr _operand2;
    }

    public class Constant : Expr
    {
        public override IEnumerable<string> Variables => new List<string> { };
        public override bool IsConstant => true;
        public override bool IsPolynom => true;

        private double value;
        public Constant(double Value)
        {
            value = Value;
        }
        //public static Constant operator +(Constant A, Constant B)
        //{
        //    return new Constant(A.value + B.value);
        //}
        //public static Constant operator -(Constant A, Constant B)
        //{
        //    return new Constant(A.value - B.value);
        //}
        //public static Constant operator *(Constant A, Constant B)
        //{
        //    return new Constant(A.value * B.value);
        //}
        //public static Constant operator /(Constant A, Constant B)
        //{
        //    return new Constant(A.value / B.value);
        //}
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return value;
        }
        public override string ToString()
        {
            return value.ToString();
        }
    }

    public class Minus : UnaryOperation
    {
        public override bool IsConstant => _operand1.IsConstant;
        public override bool IsPolynom => _operand1.IsPolynom;
        public Minus(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return _operand1.Compute(variableValues) * (-1);
        }
        override public String ToString()
        {
            return $"( - {_operand1} )";
        }
    }

    public class Add : BinaryOperation
    {

        public override bool IsConstant => _operand1.IsConstant && _operand2.IsConstant;
        public override bool IsPolynom => _operand1.IsPolynom && _operand2.IsPolynom;
        public Add(Expr A, Expr B) : base(A, B) { }
        override public String ToString()
        {
            return $"({_operand1} + {_operand2})";
        }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return _operand1.Compute(variableValues) + _operand2.Compute(variableValues);
        }
    }

    public class Sub : BinaryOperation
    {
        public override bool IsConstant => _operand1.IsConstant && _operand2.IsConstant || _operand1.ToString() == _operand2.ToString();
        public override bool IsPolynom => _operand1.IsPolynom && _operand2.IsPolynom;
        public Sub(Expr A, Expr B) : base(A, B) { }
        override public String ToString()
        {
            return $"({_operand1} - {_operand2})";
        }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return _operand1.Compute(variableValues) - _operand2.Compute(variableValues);
        }
    }

    public class Mult : BinaryOperation
    {
        public override bool IsConstant => _operand1.IsConstant && _operand2.IsConstant;
        public override bool IsPolynom => (_operand1.IsPolynom && _operand2.IsConstant) || (_operand1.IsPolynom && _operand2.IsPolynom) || (_operand1.IsConstant && _operand2.IsPolynom);
        public Mult(Expr A, Expr B) : base(A, B) { }
        override public String ToString()
        {
            return $"({_operand1} * {_operand2})";
        }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return _operand1.Compute(variableValues) * _operand2.Compute(variableValues);
        }
    }

    public class Divide : BinaryOperation
    {
        public override bool IsConstant => _operand1.IsConstant && _operand2.IsConstant;
        public override bool IsPolynom => _operand1.IsPolynom && _operand2.IsConstant;
        public Divide(Expr A, Expr B) : base(A, B) { }
        override public String ToString()
        {
            return $"({_operand1} / {_operand2})";
        }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return _operand1.Compute(variableValues) / _operand2.Compute(variableValues);
        }
    }


    public class SinFunc : Function
    {
        public override bool IsConstant => _operand1.IsConstant;

        public override bool IsPolynom => _operand1.IsConstant;
        public SinFunc(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return Math.Sin(_operand1.Compute(variableValues));
        }
        public override string ToString()
        {
            return $"sin({_operand1})";
        }
    }

    public class CosFunc : Function
    {
        public override bool IsConstant => _operand1.IsConstant;
        public override bool IsPolynom => _operand1.IsConstant;
        public CosFunc(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return Math.Cos(_operand1.Compute(variableValues));
        }
        public override string ToString()
        {
            return $"cos({_operand1})";
        }
    }

    public class TgFunc : Function
    {
        public override bool IsConstant => _operand1.IsConstant;
        public override bool IsPolynom => _operand1.IsConstant;
        public TgFunc(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return Math.Tan(_operand1.Compute(variableValues));
        }
        public override string ToString()
        {
            return $"tg({_operand1})";
        }
    }

    public class CtgFunc : Function
    {
        public override bool IsConstant => _operand1.IsConstant;
        public override bool IsPolynom => _operand1.IsConstant;
        public CtgFunc(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return 1 / Math.Tan(_operand1.Compute(variableValues));
        }
        public override string ToString()
        {
            return $"ctg({_operand1})";
        }
    }

    public class SinFunc_h : Function
    {
        public override bool IsConstant => _operand1.IsConstant;
        public override bool IsPolynom => _operand1.IsConstant;
        public SinFunc_h(Expr A) : base(A) { }
        public override double Compute(IReadOnlyDictionary<string, double> variableValues)
        {
            return Math.Sinh(_operand1.Compute(variableValues));
        }
        public override string ToString()
        {
            return $"sinh({_operand1})";
        }
    }

    //public static class Function_simple
    //{
    //    public static SinFunc Sin(Expr arg) => new SinFunc(arg);
    //    public static SinFunc_h Sinh(Expr arg) => new SinFunc_h(arg);
    //    public static CosFunc Cos(Expr arg) => new CosFunc(arg);
    //    public static TgFunc Tg(Expr arg) => new TgFunc(arg);
    //    public static CtgFunc Ctg(Expr arg) => new CtgFunc(arg);
    //}

    public class Program
    {
        public static Expr Sin(Expr A)
        {
            return new SinFunc(A);
        }
        public static Expr Cos(Expr A)
        {
            return new CosFunc(A);
        }
        public static Expr Tg(Expr A)
        {
            return new TgFunc(A);
        }
        public static Expr Ctg(Expr A)
        {
            return new CtgFunc(A);
        }
        public static Expr Sinh(Expr A)
        {
            return new SinFunc_h(A);
        }

        public static void Main()
        {
            var a = new Variable("a");
            var b = new Variable("b");
            var expr0 = new Mult(new Add(a, b), new SinFunc(new Divide(a, new Constant(2))));
            double a_value = 6.28, b_value = 0;
            //Console.Write("a = ");
            //double a_value = Convert.ToSingle(Console.ReadLine());
            //Console.Write("b = ");
            //double b_value = Convert.ToSingle(Console.ReadLine());
            var expr = (a + b) * Sin(a / 2);
            var expr1 = (a + b) * Sinh(a / 2);
            Console.Write(expr0.ToString() + " = ");
            Console.WriteLine(expr0.Compute(new Dictionary<string, double> { ["a"] = a_value, ["b"] = b_value }));
            //Console.Write(expr.ToString() + " = ");
            //Console.WriteLine(expr.Compute(new Dictionary<string, double> { ["a"] = a_value, ["b"] = b_value }));
            //Console.Write(expr.ToString() + " = ");
            //Console.WriteLine(expr1.Compute(new Dictionary<string, double> { ["a"] = a_value, ["b"] = b_value }));
            Console.ReadLine();
        }
    }
}


