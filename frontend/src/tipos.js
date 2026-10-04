// "valor" tem que ser igual ao que o C++ espera (Caminhao e Cartao sem acento)
export const TIPOS = [
  { valor: 'Carro', nome: 'Carro', icone: '🚗' },
  { valor: 'Moto', nome: 'Moto', icone: '🏍️' },
  { valor: 'Caminhao', nome: 'Caminhão', icone: '🚚' },
]

export const METODOS = [
  { valor: 'Dinheiro', nome: 'Dinheiro', icone: '💵' },
  { valor: 'Cartao', nome: 'Cartão', icone: '💳' },
  { valor: 'Pix', nome: 'Pix', icone: '⚡' },
]

export function infoTipo(valor) {
  return TIPOS.find((tipo) => tipo.valor === valor) || { valor, nome: valor, icone: '🚘' }
}

export function formatarDinheiro(valor) {
  return valor.toLocaleString('pt-BR', { style: 'currency', currency: 'BRL' })
}
