--1
SELECT *
FROM dep; 

--2
SELECT nome, funcao, sal, ndep 
FROM emp;

--3
SELECT *
FROM dep
ORDER BY ndep;

--4
SELECT DISTINCT funcao
FROM emp;

--5
SELECT nome, funcao, premios
FROM emp
WHERE premios IS NOT NULL;

--6
SELECT *
FROM emp
WHERE ndep IN (10, 30);

--7
SELECT nome
FROM emp
WHERE funcao = 'Analista' AND ndep = 20;

--8
nome, funcao
FROM emp
WHERE nome LIKE 'A%us%' OR nome LIKE 'R%ei%';

--9
SELECT nome, funcao, (sal * 14 + COALESCE(premios, 0)) AS "Remuneração Anual"
FROM emp;

--10
SELECT e.nome, e.sal, e.ndep, d.nome
FROM emp AS e, dep AS d
WHERE (e.nome LIKE 'A% R%') AND (e.ndep = d.ndep);

--11
SELECT e.nome, e.sal, d.nome, d.local
FROM emp AS e, dep AS d 
WHERE sal < 150000 AND e.ndep = d.ndep;

--12
SELECT DISTINCT e.funcao, dsc.escalao
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
ORDER BY dsc.escalao, e.funcao;

--13
SELECT DISTINCT e.funcao, dsc.escalao
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
ORDER BY dsc.escalao, e.funcao;

--14
SELECT e.nome, e.funcao, d.nome, e.sal
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
JOIN dep AS d
ON e.ndep = d.ndep
WHERE escalao = 4;

--15
SELECT e.nome, e.funcao, e.sal, d.local
FROM emp AS e
JOIN dep AS d
ON e.ndep = d.ndep
WHERE d.local = 'Coimbra' AND e.sal > 150000;

--16
SELECT e.nome, e.funcao, dsc.escalao, d.nome
FROM emp AS e
JOIN descontos as dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
JOIN dep AS d
ON e.ndep = d.ndep
WHERE e.funcao != 'Continuo'
ORDER BY dsc.escalao DESC, e.nome;

--17
SELECT NULL AS NOMES, 0 AS NUMEROS
UNION ALL
SELECT 'DEPARTAMENTOS:', 1
UNION ALL
SELECT d.nome, d.ndep
FROM dep AS d
UNION ALL
SELECT NULL, 999
UNION ALL
SELECT 'EMPREGADOS:', 1000
UNION ALL
SELECT e.nome, e.nemp
FROM emp AS e
ORDER BY NUMEROS;

--18 (O OPERADOR MINUS NÃO EXISTE NO PostgreSQL)
SELECT d.ndep, d.nome, d.local, e.nome
FROM dep AS d
LEFT JOIN emp AS e
ON e.ndep = d.ndep
WHERE e.nome IS NULL;

--19
SELECT e.nome
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
hms_db-# WHERE dsc.escalao >= 2;

--20
SELECT COUNT(*) AS "Total de empregados"
FROM emp;

--21
SELECT  
    COUNT(*) AS "Total de Empregados", 
    ROUND(AVG(sal), 3) AS "Salário médio mensal", 
    SUM(sal*14 + premios) AS "Remuneração total anual"
FROM emp;

--22
SELECT 
    e.nome, 
    e.funcao, 
    e.sal, 
    ROUND((e.sal * 1.1355), 1) AS "Salário com aumento"
FROM emp AS e
WHERE e.funcao = 'Continuo';

--23
SELECT CONCAT('>>>> ', e.nome, ' ', REPEAT('<', 20-LENGTH(e.nome)))
FROM emp AS e
ORDER BY e.nome;

--24
SELECT MIN(sal), MAX(sal), ROUND(AVG(SAL), 3)
FROM emp;

--25
SELECT funcao, COUNT(*)
FROM emp
GROUP BY funcao
HAVING funcao like '%o' OR funcao = 'Analista';

--26
SELECT funcao, FLOOR(AVG(sal))
FROM emp
GROUP BY funcao
HAVING COUNT(1) > 1;

--27
SELECT dsc.escalao, COUNT(*)
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
JOIN dep AS d ON e.ndep = d.ndep
WHERE d.local = 'Coimbra'
GROUP BY dsc.escalao
ORDER BY escalao;

--28
SELECT d.nome, COUNT(1), MAX(e.sal), MIN(e.sal)
FROM dep AS d
JOIN emp AS e
ON e.ndep = d.ndep
GROUP BY d.ndep
HAVING AVG(e.sal) > 200000;

--29
SELECT dsc.escalao, COUNT(DISTINCT d.ndep)
FROM emp AS e
JOIN descontos AS dsc
ON e.sal BETWEEN dsc.salinf AND dsc.salsup
JOIN dep AS d
ON e.ndep = d.ndep
GROUP BY dsc.escalao
HAVING dsc.escalao IN (1, 2, 3);

--30 
SELECT e.nome, e.ndep, e.sal
FROM emp AS e
WHERE e.sal BETWEEN 150000 AND 300000;

--31
SELECT nome, funcao
FROM emp
WHERE nome LIKE '%v%' OR nome LIKE '%u%';

--32
SELECT nome,  ROUND(0.1 * (sal * 14 + premios)) AS "10% Sal. Anual", premios
FROM emp
WHERE premios < 0.1 * (sal * 14 + premios)
ORDER BY nome;

--33
 SELECT e.nome, e.funcao, d.nome
FROM emp AS e
JOIN dep AS d
ON e.ndep = d.ndep
ORDER BY d.nome, e.nome;

--34
SELECT e.nome, e.funcao, d.nome
FROM dep AS d
LEFT JOIN emp AS e
ON e.ndep = d.ndep
ORDER BY d.nome, e.nome;

--35
 SELECT e1.nome, e2.nome, dsc.escalao, d.local
FROM emp AS e1
JOIN emp AS e2
ON e1.encar = e2.nemp
JOIN descontos AS dsc
ON e1.sal BETWEEN dsc.salinf AND dsc.salsup
JOIN dep AS d
ON e1.ndep = d.ndep
WHERE e1.sal > 150000;

--36
 SELECT UPPER(e.nome), LOWER(e.funcao), INITCAP(d.nome)
FROM emp AS e
JOIN dep AS d
ON e.ndep = d.ndep
WHERE e.funcao = 'Vendedor';

--37
SELECT e1.nome AS "Encarregado", e2.nome AS "Empregado"
FROM emp AS e1
JOIN emp AS e2
ON e2.encar = e1.nemp
ORDER BY e1.nome, e2.nome;

--38
SELECT COUNT(1) AS "Empregados sem premio"
FROM emp
WHERE premios IS NULL;

--39
SELECT d.ndep, MAX(e.sal) - MIN(e.sal) AS "DIFERENCA"
FROM dep AS d
JOIN emp AS e
ON e.ndep = d.ndep
GROUP BY d.ndep
ORDER BY d.ndep;

--40
SELECT enc.nemp, MIN(e.sal)
FROM emp AS enc
RIGHT JOIN emp AS e
ON e.encar = enc.nemp
GROUP BY enc.nemp
HAVING MIN(e.sal) >= 200000
ORDER BY MIN(e.sal);

--41
SELECT e.ndep, e.funcao, MAX(e.sal), MIN(e.sal), ROUND(AVG(e.sal)), COUNT(1)
FROM emp AS e
WHERE e.nome <> 'Jorge Sampaio'
GROUP BY e.funcao, e.ndep
HAVING MAX(e.sal) > 200000
ORDER BY e.ndep, MAX(e.sal);

--42
SELECT enc.nome, d.local, COUNT(e.nemp), SUM(e.sal)
FROM emp AS enc
JOIN emp AS e
ON enc.nemp = e.encar
JOIN dep AS d 
ON enc.ndep = d.ndep
GROUP BY enc.nome, d.ndep 
HAVING d.local = 'Coimbra';

--43
SELECT MIN(sal)
FROM emp;

--44
SELECT nome, funcao, sal
FROM emp
WHERE funcao = (
                SELECT funcao 
                FROM emp 
                WHERE nome = 'Olga Costa'
                );

--45
SELECT e.nome, e.sal, e.ndep
FROM emp AS e
WHERE e.sal IN (
                SELECT MAX(e.sal)
                FROM emp AS e
                JOIN dep AS d
                ON e.ndep = d.ndep
                GROUP BY d.ndep
                );

--46
SELECT e.nome, e.sal, e.funcao, e.ndep
FROM emp AS e
WHERE e.sal > ANY (
                    SELECT e.sal 
                    FROM emp AS e 
                    JOIN dep AS d 
                    ON d.ndep = e.ndep 
                    WHERE d.ndep = 30
                    )
ORDER BY e.sal DESC;

--47
SELECT e.nome, e.sal, e.funcao, e.ndep
FROM emp AS e
WHERE e.sal > ALL (
                    SELECT e.sal
                    FROM emp AS e
                    JOIN dep AS d ON e.ndep = d.ndep
                    WHERE d.nome = 'Vendas'
                    )
ORDER BY e.sal DESC;

--48
SELECT e.nome, e.sal, e.funcao, d.nome
FROM emp AS e
JOIN dep AS d
ON e.ndep = d.ndep
WHERE e.sal > (
                SELECT AVG(e.sal) 
                FROM emp AS e 
                JOIN dep AS d 
                ON e.ndep = d.ndep 
                WHERE d.nome = 'Vendas'
                )
ORDER BY e.nome;

--49
SELECT TO_CHAR('2005-01-01'::DATE, 'Day') AS "Dia da Semana";

--50
SELECT nome, data_entrada
FROM emp
WHERE data_entrada > '1993-01-01'::DATE;

--51
INSERT INTO emp2 (nemp, nome, funcao, encar, data_entrada, sal, premios, ndep)
VALUES (5555, 'CHICO FININHO', 'Cantor', null, CURRENT_DATE, 100000, null, 40);

--52
INSERT INTO emp2 (nemp, nome, funcao, encar, data_entrada, sal, premios, ndep)
SELECT nemp, UPPER(nome), funcao, encar, data_entrada, GREATEST(sal, 70001), premios, ndep
FROM emp
WHERE nemp NOT IN (1902, 1369);

--53
DELETE FROM emp2
WHERE ndep = 20;

--54
DELETE FROM emp2
WHERE nemp = 1654;

--55
UPDATE emp2
SET
  nome = UPPER(nome),
  sal = 1.1 * (
    SELECT AVG(sal)
    FROM emp e
    WHERE e.ndep = emp2.ndep
    AND e.sal <= emp2.sal
  ),
  premios = 1.5 * (
    SELECT AVG(premios)
    FROM emp e
    WHERE e.ndep = emp2.ndep
  ),
  ndep = CASE
    WHEN ndep IN (10, 20, 30) THEN ndep + 10
    ELSE ndep
  END
WHERE sal <= (
  SELECT MAX(sal)
  FROM emp e
  WHERE e.ndep = emp2.ndep
);

--56
DROP TABLE emp2;

--57
