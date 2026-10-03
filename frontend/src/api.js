const SEM_SERVIDOR = 'Não foi possível falar com o servidor C++. Ele está rodando com "estacionamento web"?'

async function chamar(rota, opcoes) {
  let resposta
  let dados

  try {
    resposta = await fetch('/api' + rota, opcoes)
    dados = await resposta.json()
  } catch {
    throw new Error(SEM_SERVIDOR)
  }

  if (!resposta.ok) {
    throw new Error(dados.erro || 'Erro desconhecido no servidor')
  }

  return dados
}

export function buscarEstado() {
  return chamar('/estado')
}

// manda como formulario pq o httplib ja le isso sozinho, sem o C++ precisar entender JSON
export function enviar(rota, campos) {
  return chamar(rota, { method: 'POST', body: new URLSearchParams(campos) })
}
