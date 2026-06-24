# Security Auditing — Exam 2022-06-24 (EN) — Proposed answers

## ULTRA ALERTA, QUEM NÃO LER ESTE ALERTA MORRE E FICA MORTO
Esta resolução foi feita pelo claude pq a resolução originalmente associada ao enunciado pareceu me imcompleta, e desatualizada com a matéria dada em 25/26.

A resolução foi gerada pelo Claude com um único prompt, dado o contexto da consulta legal deste ano (25/26).

Não me responsabilizo por esta resolução!!

---

Answers grounded in the class materials in `OpenBook/2025-2026/`.
Slide refs use the **printed lesson number** (note the file/lesson off‑by‑one:
`2026-AS-T7-v1.pdf` is printed as *Lesson #8*).

> Correction applied (per your note): in Q1, `A.15.1.1` (ISO 27001:2013) is the
> renumbered **A.5.19** in ISO 27001/27002:2022. The 2013 *objective* "A.15.1
> Information security in supplier relationships" is now the single 2022 *control*
> **A.5.19 – Information Security in Supplier Relationships** (see T4, ISO 27002 list).

---

## 1. (1.5) Control contributing to A.5.19 – Information Security in Supplier Relationships

**Objective of A.5.19:** ensure the protection of the organization's assets that
are accessible to, or affected by, suppliers — by defining and applying
information‑security requirements *before and throughout* the supplier
relationship (T4 lists A.5.19–A.5.23 as the supplier/cloud controls; T7/NIS2
stresses mandatory supplier risk management).

**Example of a control (give one as the main answer, the rest reinforce it):**

- **Documented supplier‑security policy + supplier risk classification** *(the most
  direct control — this *is* A.5.19)*: identify and classify suppliers by the
  criticality of the assets they touch, and define the minimum security
  requirements each must satisfy **before** access is granted.
- **Security clauses in supplier agreements (A.5.20):** contractual obligations to
  encrypt data, report incidents within a fixed deadline, restrict sub‑processing,
  and return/securely delete assets at contract end.
- **Confidentiality / Non‑Disclosure Agreement (A.6.6):** signed by the supplier and
  its staff before access to sensitive information — gives the organization legal
  recourse against misuse of trusted assets *(this is your `kiko.md` example — valid)*.
- **Right‑to‑audit clause + evidence of assurance:** the right to audit the supplier
  (or require their own ISO 27001 cert / SOC 2 report) and to **monitor and review**
  supplier services over time (A.5.22).
- **Least‑privilege, time‑limited, logged access** for supplier accounts.

*One‑sentence model answer:* "Require, by contract, that every supplier with access
to organizational assets meets a documented set of minimum security requirements
(e.g. encryption, breach‑notification deadlines, NDA, right‑to‑audit) agreed **before**
access is granted — operationalised through a supplier‑security policy and a
risk‑based supplier register."

---

## 2. (1.5) Main functions of the "CSIRT Network" (Reg. (EU) 2019/881 — Cybersecurity Act)

**Rigor note (worth stating — it's the thing the question gets slightly wrong):**
the **CSIRTs Network is not created by the Cybersecurity Act**. It was established by
the **NIS Directive** (Art. 12 of Dir. (EU) 2016/1148, now Art. 15 of **NIS2**, Dir.
(EU) 2022/2555). Regulation (EU) **2019/881 (Cybersecurity Act)** gives **ENISA** a
**permanent, reinforced mandate** and tasks it with **supporting operational
cooperation and providing the secretariat of the CSIRTs Network** (Art. 7), plus the
EU‑wide certification framework (T7: "ENISA is now a central hub for exchanging threat
intelligence and best practices"; "Coordinating Incident Response & Threat Intelligence").

**Composition:** the national CSIRTs of the Member States + CERT‑EU, with ENISA as
secretariat.

**Main function — promote fast, effective *operational cooperation* between national
CSIRTs.** Concretely:

- **Trust & capability building:** exchange information on each CSIRT's services,
  operations and cooperation capabilities.
- **Incident information sharing (voluntary):** share technical details / IoCs on
  individual and **cross‑border** incidents needed to handle them.
- **Mutual assistance & coordinated response** to large‑scale cross‑border incidents
  that affect more than one Member State.
- **Threat intelligence & best‑practice sharing**, including lessons learned from
  cyber exercises.
- **ENISA operational support:** aggregate and analyse national incident reports,
  ensure efficient information flow, and provide technical support / **ex‑post
  technical inquiries** on request.

**Correction to your `kiko.md` draft:** the "report within 24 h" rule is the **NIS2**
obligation for an entity to give an *early warning to its **national** CSIRT /
competent authority* (T7) — that is **not** the function of the CSIRT *Network*. The
Network is cooperation **between** the national CSIRTs, not a single mailbox that
organizations report incidents to.

---

## 3. (1.5) Four most important controls for the (System/)Application Domain — justified

**Framing:** the *System/Application Domain* is the 7th of the **Seven Domains of a
Typical IT Infrastructure** (T2): mainframes, application & web servers, databases and
proprietary software that users access. The course's stated posture for this domain is
to **"harden servers to an authorized baseline, configured to policies and standards
with controls"** and to keep it **audit‑ready**. The four controls below map to ISO
27002:2022 **Technological controls (A.8)** from T4.

1. **Secure baseline configuration / hardening + configuration & change management**
   (A.8.9; T2: "harden to authorized baseline" + Config/Change Mgmt).
   *Why:* insecure default/drifted configuration is the most exploited weakness in this
   domain. A locked‑down baseline plus a process where changes are **requested,
   evaluated, authorized and tracked** keeps systems in a known‑good, policy‑compliant
   state and makes the domain auditable.

2. **Access control & strong authentication** — least privilege, RBAC, secure auth/MFA,
   privileged‑access management (A.8.2, A.8.3, A.8.5; A.5.15).
   *Why:* applications hold the business logic and the data. Restricting *who* can reach
   *which* functions/data and enforcing strong authentication limits the blast radius of
   a compromised account and gives accountability.

3. **Technical vulnerability & patch management** (+ secure SDLC / app‑security
   requirements / secure coding for in‑house software) (A.8.8; A.8.25/8.26/8.28).
   *Why:* app/web/DB servers are directly exposed. Timely patching and secure coding
   (e.g. input validation against SQLi/XSS) close the most common exploited
   vulnerabilities before attackers reach them.

4. **Logging, monitoring & audit trails** (A.8.15, A.8.16; T2 "audit log settings").
   *Why:* "what isn't logged didn't happen." System/application logs are essential to
   detect anomalies, support incident response and forensics, prove compliance, and hold
   users accountable — directly enabling the audit itself.

*Equally defensible swaps (it's a "controls **you** consider" question):* data
encryption at rest/in transit (A.8.24), protection against malware (A.8.7), information
backup (A.8.13).

---

## 4. (1.5) Two situations showing the importance of a Code of Ethics in IT security auditing

**Framing:** the course defines an audit as an **independent, objective** evaluation and
states explicitly that *"auditors never audit applications, processes or systems they
designed or created"* (T1). A code of ethics (ISACA, (ISC)²) operationalizes
**independence, objectivity, integrity, confidentiality and competence**.

**Situation 1 — Independence / conflict of interest (self‑review).**
The auditor is assigned to audit a system, control or vendor that **they themselves
designed/implemented**, or in which they have a personal/financial interest (e.g. they
hold shares in the audited vendor, or a relative leads the audited team). Without a code
of ethics requiring them to **declare the conflict and recuse**, the "independent" review
is biased — they would be judging their own work — and the assurance value of the audit
collapses. The code obliges disclosure and reassignment, preserving objectivity (directly
the T1 rule).

**Situation 2 — Confidentiality / misuse of privileged access.**
To run the audit, the auditor gains privileged access to sensitive data (PII, financials,
trade secrets) and discovers **unpatched critical vulnerabilities**. A code of ethics binds
the auditor to confidentiality and to **not exploiting** that knowledge — not copying or
leaking data, not insider‑trading, not selling/exploiting the vulnerabilities, and
disclosing findings responsibly only to authorized stakeholders. Without it, the very
access the audit *requires* becomes a major risk to the client.

*Alternative situation — Integrity/objectivity under pressure:* management pressures or
bribes the auditor to **omit a critical finding** so the company "passes" and signs a
lucrative contract. The code requires honest, accurate and complete reporting regardless
of pressure.

---

### Note on the provided DOCX solution
It is not grossly wrong, but it reads as a generic AI answer that doesn't anchor to *these*
class materials:
- **Q1** keeps the obsolete 2013 label/wording (should be A.5.19 / "Information Security in
  Supplier Relationships").
- **Q2** is broadly accurate but loosely attributes the network's creation to the
  Cybersecurity Act; the Network is a **NIS/NIS2** construct that the Act's **ENISA** mandate
  *supports*. State that distinction.
- **Q3** picks generic web‑app controls (input validation, RBAC, encryption, logging) and
  **omits the course's headline control for this domain**: server hardening to a secure
  baseline + configuration/change management. Lead with that.
- **Q4** uses confidentiality + management pressure (fine) but **misses the course's explicit
  independence/self‑review principle** from T1 — the strongest, most course‑specific example.
