### Why?

Este merge request propõe a introdução de uma nova estrutura de logger (usando PImpl e spdlog) e a refatoração das classes de transporte (como TCPTransport e SerialTransport) para também seguirem o padrão PImpl. Com isso:
    Encapsulamento dos detalhes de implementação.
    Consistência e facilidade de manutenção ao unificar o modo como os logs são tratados em todo o projeto.
    Flexibilidade para futuras trocas de biblioteca de log ou ajustes internos de transporte, sem impactar as interfaces públicas.

### What?

Logger: Foi criada uma classe Logger que usa PImpl e integra spdlog. Ela permite categorizar mensagens via CommandContext e padroniza o log no projeto.
Transporte: As classes TCPTransport e SerialTransport foram atualizadas para também seguirem PImpl, reduzindo dependências e mantendo uma interface pública estável. Além disso, elas passam a usar o novo logger para registrar eventos e erros de forma consistente.

Depends-on: <Link to merge request which this one depends on. If is not the case remove this line.> \
Blocks: <Link to merge request which this one blocks. If is not the case remove this line.>

### Checklist

- [ ] Changelog is updated: <Link to merge request where changelog was updated.>
- [ ] Test build: <Path to test build file in server>

/assign me
/request_review @alexrosa
