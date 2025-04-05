# AST 구조 분석기

C 소스 코드에서 함수 정의, 파라미터, 반환 타입, 조건문 수를 자동 추출하는 AST(Abstract Syntax Tree) 기반 분석기입니다. `pycparser`를 이용해 C 코드의 AST를 JSON으로 변환한 후, 이를 분석하여 함수 정보를 정리합니다.

## 프로젝트 구성

```
├── ast.json         # pycparser로 생성한 AST JSON 파일
├── target.txt       # 전처리된 텍스트 파일 (자동 생성됨)
├── analyzer.c           # AST 분석기 메인 소스코드
├── README.md        # 프로젝트 설명 파일
```

## 실행 방법

1. C 소스 파일을 `pycparser`를 사용하여 AST JSON 형식으로 변환:

   ```python
   from pycparser import parse_file, c_generator
   import json
   from pycparser.c_ast import NodeVisitor

   ast = parse_file('your_code.c', use_cpp=True)
   with open('ast.json', 'w') as f:
       f.write(str(ast))  # 또는 custom visitor를 활용한 json.dumps()
   ```

2. `main.c` 컴파일 및 실행:

   ```bash
   gcc -o analyzer main.c
   ./analyzer
   ```

3. 분석 결과 출력 예시:

   ```
   1. int* main()
       : has 0 parameters and 0 if statements
   2. void get_token()
       : has 0 parameters and 7 if statements

   The number of functions : 36
   ```

## 기능 설명

- 함수 정의 자동 탐지
- 반환형 및 함수 이름 추출
- 파라미터 타입 및 변수명 파싱
- 함수 내 if 문 개수 분석
- 포인터 타입 지원 (예: `char*`)
