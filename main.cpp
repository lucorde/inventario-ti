#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

struct Equipamento {
    int id;
    string tipo;
    string modelo;
    string patrimonio;
    string status;
    string responsavel; // quem esta usando (vazio se disponivel)
};

const string ARQUIVO = "equipamentos.txt";

// ---------- utilidades ----------

bool lerInteiro(int& valor) {
    cin >> valor;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

string paraMinusculo(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return tolower(c); });
    return s;
}

Equipamento* acharPorId(vector<Equipamento>& lista, int id) {
    for (Equipamento& e : lista) {
        if (e.id == id) return &e;
    }
    return nullptr;
}

void imprimir(const Equipamento& e) {
    cout << "ID: " << e.id
         << " | Tipo: " << e.tipo
         << " | Modelo: " << e.modelo
         << " | Patrimonio: " << e.patrimonio
         << " | Status: " << e.status;
    if (!e.responsavel.empty()) {
        cout << " | Responsavel: " << e.responsavel;
    }
    cout << "\n";
}

// ---------- arquivo ----------

void salvar(const vector<Equipamento>& lista) {
    ofstream arq(ARQUIVO);
    if (!arq) {
        cout << "Erro ao salvar o arquivo.\n";
        return;
    }
    for (const Equipamento& e : lista) {
        arq << e.id << ';' << e.tipo << ';' << e.modelo << ';'
            << e.patrimonio << ';' << e.status << ';'
            << e.responsavel << '\n';
    }
}

void carregar(vector<Equipamento>& lista) {
    ifstream arq(ARQUIVO);
    if (!arq) return;

    string linha;
    while (getline(arq, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string idTexto;
        Equipamento e;

        getline(ss, idTexto, ';');
        getline(ss, e.tipo, ';');
        getline(ss, e.modelo, ';');
        getline(ss, e.patrimonio, ';');
        getline(ss, e.status, ';');
        getline(ss, e.responsavel, ';');

        try {
            e.id = stoi(idTexto);
        } catch (...) {
            continue;
        }
        lista.push_back(e);
    }
}

// ---------- funcoes do menu ----------

void cadastrar(vector<Equipamento>& lista) {
    Equipamento e;

    cout << "\n--- Cadastrar equipamento ---\n";
    cout << "ID: ";
    if (!lerInteiro(e.id)) {
        cout << "ID invalido. Digite apenas numeros.\n";
        return;
    }
    if (acharPorId(lista, e.id) != nullptr) {
        cout << "Ja existe um equipamento com esse ID.\n";
        return;
    }

    cout << "Tipo (laptop, impressora, monitor...): ";
    getline(cin, e.tipo);
    cout << "Modelo: ";
    getline(cin, e.modelo);
    cout << "Numero de patrimonio: ";
    getline(cin, e.patrimonio);

    e.status = "disponivel";
    e.responsavel = "";
    lista.push_back(e);
    salvar(lista);
    cout << "Equipamento cadastrado com sucesso!\n";
}

void listar(const vector<Equipamento>& lista) {
    cout << "\n--- Equipamentos cadastrados ---\n";
    if (lista.empty()) {
        cout << "Nenhum equipamento cadastrado ainda.\n";
        return;
    }
    for (const Equipamento& e : lista) {
        imprimir(e);
    }
}

void buscar(const vector<Equipamento>& lista) {
    cout << "\n--- Buscar equipamento ---\n";
    cout << "Digite o ID ou parte do tipo/modelo: ";
    string termo;
    getline(cin, termo);
    termo = paraMinusculo(termo);

    int encontrados = 0;
    for (const Equipamento& e : lista) {
        bool bateId = (to_string(e.id) == termo);
        bool bateTipo = paraMinusculo(e.tipo).find(termo) != string::npos;
        bool bateModelo = paraMinusculo(e.modelo).find(termo) != string::npos;

        if (!termo.empty() && (bateId || bateTipo || bateModelo)) {
            imprimir(e);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        cout << "Nenhum equipamento encontrado.\n";
    }
}

void atribuir(vector<Equipamento>& lista) {
    cout << "\n--- Atribuir equipamento a um usuario ---\n";
    cout << "ID do equipamento: ";
    int id;
    if (!lerInteiro(id)) {
        cout << "ID invalido.\n";
        return;
    }

    Equipamento* e = acharPorId(lista, id);
    if (e == nullptr) {
        cout << "Equipamento nao encontrado.\n";
        return;
    }
    if (e->status != "disponivel") {
        cout << "Esse equipamento nao esta disponivel (status: "
             << e->status << ").\n";
        return;
    }

    cout << "Nome do usuario: ";
    getline(cin, e->responsavel);
    e->status = "em uso";
    salvar(lista);
    cout << "Equipamento atribuido a " << e->responsavel << ".\n";
}

void devolver(vector<Equipamento>& lista) {
    cout << "\n--- Devolver equipamento ---\n";
    cout << "ID do equipamento: ";
    int id;
    if (!lerInteiro(id)) {
        cout << "ID invalido.\n";
        return;
    }

    Equipamento* e = acharPorId(lista, id);
    if (e == nullptr) {
        cout << "Equipamento nao encontrado.\n";
        return;
    }
    if (e->status != "em uso") {
        cout << "Esse equipamento nao esta em uso.\n";
        return;
    }

    e->status = "disponivel";
    e->responsavel = "";
    salvar(lista);
    cout << "Equipamento devolvido e marcado como disponivel.\n";
}

void remover(vector<Equipamento>& lista) {
    cout << "\n--- Remover equipamento ---\n";
    cout << "ID do equipamento: ";
    int id;
    if (!lerInteiro(id)) {
        cout << "ID invalido.\n";
        return;
    }

    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].id == id) {
            imprimir(lista[i]);
            cout << "Tem certeza que deseja remover? (s/n): ";
            string resposta;
            getline(cin, resposta);

            if (resposta == "s" || resposta == "S") {
                lista.erase(lista.begin() + i);
                salvar(lista);
                cout << "Equipamento removido.\n";
            } else {
                cout << "Remocao cancelada.\n";
            }
            return;
        }
    }
    cout << "Equipamento nao encontrado.\n";
}

int main() {
    vector<Equipamento> equipamentos;
    carregar(equipamentos);

    int opcao = -1;

    while (opcao != 0) {
        cout << "\n=== Inventario de TI ===\n";
        cout << "1 - Cadastrar equipamento\n";
        cout << "2 - Listar equipamentos\n";
        cout << "3 - Buscar equipamento\n";
        cout << "4 - Atribuir a um usuario\n";
        cout << "5 - Devolver equipamento\n";
        cout << "6 - Remover equipamento\n";
        cout << "0 - Sair\n";
        cout << "Escolha: ";

        if (!lerInteiro(opcao)) {
            opcao = -1;
            cout << "Digite apenas numeros.\n";
            continue;
        }

        switch (opcao) {
            case 1: cadastrar(equipamentos); break;
            case 2: listar(equipamentos); break;
            case 3: buscar(equipamentos); break;
            case 4: atribuir(equipamentos); break;
            case 5: devolver(equipamentos); break;
            case 6: remover(equipamentos); break;
            case 0: cout << "Ate mais!\n"; break;
            default: cout << "Opcao invalida.\n";
        }
    }
    return 0;
}