# Tenge Language - Updated Keywords

**Официальные латинские казахские ключевые слова**

## 📋 Обновленные ключевые слова

### **Основные ключевые слова**

| Старое (кириллица) | Новое (латиница) | Значение | Английский эквивалент |
|-------------------|------------------|----------|----------------------|
| `jasa` | `jasau` | создавать | create/declare |
| `bekit` | `bekit` | постоянный | constant |
| `atqar'm` | `atqar` | функция | function |
| `qaıtar` | `qaytar` | возвращать | return |
| `eger` | `eger` | если | if |
| `áıtpece` | `aitpese` | иначе | else |
| `ázirshe` | `azirshe` | пока | while |
| `jan` | `jan` | душа (истина) | true |
| `j'n` | `jin` | демон (ложь) | false |
| `kórset` | `korset` | показать | show/print |

### **Типы данных**

| Старое (кириллица) | Новое (латиница) | Значение | Английский эквивалент |
|-------------------|------------------|----------|----------------------|
| `san` | `san` | число | number/int |
| `aqsha` | `aqsha` | деньги | money/decimal |
| `jol` | `jol` | путь/строка | string |
| `tańba` | `tanba` | знак равенства | equal |
| `aqıqat` | `aqiqat` | истина | boolean |
| `j'i'm` | `jim` | стрелка | arrow |

## 🔄 Изменения в коде

### **1. Token.go**
```go
// Старые константы
JASA    = "jasa"
ATQARM  = "atqar'm"
QAITAR  = "qaıtar"
AITPECE = "áıtpece"
AZIRSHE = "ázirshe"
JYN     = "j'n"
KORSET  = "kórset"
TANBA   = "tańba"
AQIQAT  = "aqıqat"
JYIM    = "j'i'm"

// Новые константы
JASA    = "jasau"
ATQAR   = "atqar"
QAYTAR  = "qaytar"
AITPESE = "aitpese"
AZIRSHE = "azirshe"
JIN     = "jin"
KORSET  = "korset"
TANBA   = "tanba"
AQIQAT  = "aqiqat"
JIM     = "jim"
```

### **2. Lexer.go**
```go
// Обновленный словарь ключевых слов
var keywords = map[string]token.TokenType{
    "jasau":   token.JASA,     // create/declare
    "bekit":   token.BEKIT,    // constant
    "atqar":   token.ATQAR,    // function
    "qaytar":  token.QAYTAR,   // return
    "eger":    token.EGER,     // if
    "aitpese": token.AITPESE,  // else
    "azirshe": token.AZIRSHE,  // while
    "jan":     token.JAN,      // true
    "jin":     token.JIN,      // false
    "korset":  token.KORSET,   // show/print
    "san":     token.SAN,      // number/int
    "aqsha":   token.AQSHA,    // money/decimal
    "jol":     token.JOL,      // string
    "tanba":   token.TANBA,    // equal
    "aqiqat":  token.AQIQAT,   // boolean
    "jim":     token.JIM,      // arrow
}
```

### **3. AST.go**
```go
// Обновленные структуры
type JasauStatement struct {    // было JasaStatement
    Token token.Token
    Name  *Identifier
    Type  *TypeNode
    Value Expression
}

type QaytarStatement struct {   // было QaıtarStatement
    Token       token.Token
    ReturnValue Expression
}
```

### **4. Object.go**
```go
// Обновленные типы объектов
type Aqiqat struct {           // было Aqıqat
    Value bool
}

type QaytarValue struct {       // было QaıtarValue
    Value Object
}

// Обновленные синглтоны
var (
    NULL = &Null{}
    JAN  = &Aqiqat{Value: true}   // "jan" - душа (истина)
    JIN  = &Aqiqat{Value: false} // "jin" - демон (ложь)
)
```

## 📝 Примеры использования

### **Объявление переменных**
```tenge
// Старый синтаксис
jasa name: jol = "Hello"
bekit PI: aqsha = 3.14159

// Новый синтаксис
jasau name: jol = "Hello"
bekit PI: aqsha = 3.14159
```

### **Функции**
```tenge
// Старый синтаксис
atqar'm calculate(x: san): san {
    qaıtar x * 2
}

// Новый синтаксис
atqar calculate(x: san): san {
    qaytar x * 2
}
```

### **Условные операторы**
```tenge
// Старый синтаксис
eger x > 0 {
    kórset "Positive"
} áıtpece {
    kórset "Negative"
}

// Новый синтаксис
eger x > 0 {
    korset "Positive"
} aitpese {
    korset "Negative"
}
```

### **Циклы**
```tenge
// Старый синтаксис
ázirshe i < 10 {
    kórset i
    i = i + 1
}

// Новый синтаксис
azirshe i < 10 {
    korset i
    i = i + 1
}
```

## ✅ Преимущества обновления

1. **Соответствие стандарту** - Использование официального латинского казахского алфавита
2. **Читаемость** - Более понятные и стандартизированные слова
3. **Совместимость** - Соответствие международным стандартам
4. **Развитие языка** - Поддержка современного казахского языка

## 🔧 Миграция

Для миграции существующего кода:

1. **Автоматическая замена** - Используйте поиск и замену в IDE
2. **Поэтапная миграция** - Обновляйте файлы постепенно
3. **Тестирование** - Проверяйте работоспособность после изменений

## 📚 Дополнительные ресурсы

- [Официальный латинский казахский алфавит](https://www.akorda.kz/ru/legal_acts/decrees/o-vnesenii-izmeneniya-v-ukaz-prezidenta-respubliki-kazahstan-ot-26-oktyabrya-2017-goda-569-o-perevode-alfavita-kazahskogo-yazyka-s-kirillicy-na-latinskuyu-grafiku)
- [Документация Tenge Language](README.md)
- [Примеры кода](examples/)

