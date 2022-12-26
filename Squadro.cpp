#include "Squadro.h"
#include "ui_Squadro.h"
#include "Cell.h"
#include "Player.h"

#include <QDebug>
#include <QMessageBox>
#include <QSignalMapper>

/**
 * Nomes: Lucas Rafael Alves de Souza; Sérgio Henrique Mendes de Assis.
 * 28/11/2022
 */
Squadro::Squadro(QWidget *parent)
    : QMainWindow(parent),
        ui(new Ui::Squadro),
        m_player(nullptr) {

    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    QSignalMapper* map = new QSignalMapper(this);
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            QString cellName = QString("cell%1%2").arg(row).arg(col);
            Cell* cell = this->findChild<Cell*>(cellName);
            m_board[row][col] = cell;

            if (cell != nullptr) {
                int id = row * 7 + col;
                map->setMapping(cell, id);
                QObject::connect(cell, SIGNAL(clicked(bool)), map, SLOT(map()));
            }
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
#else
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
#endif

    // When the turn ends, switch the player.
    QObject::connect(this, SIGNAL(turnEnded()), this, SLOT(switchPlayer()));

    // Connect the red player counts.
    Player* red = Player::player(Player::Red);
    QObject::connect(red, SIGNAL(countChanged(int)), this, SLOT(updateStatusBar()));

    // Connect the blue player counts.
    Player* blue = Player::player(Player::Blue);
    QObject::connect(blue, SIGNAL(countChanged(int)), this, SLOT(updateStatusBar()));

    // Reset.
    this->reset();

    // Adjust window.
    this->adjustSize();
    this->setFixedSize(this->size());
}

Squadro::~Squadro() {
    delete ui;
}

void Squadro::play(int id) {
    int row = id / 7;
    int col = id % 7;
    int n_col;
    int n_row;
    int i = 0;
    int fim = 0;
    Cell* cell = m_board[row][col];
    Player* red = Player::player(Player::Red);
    Player* blue = Player::player(Player::Blue);

    Q_ASSERT(cell != nullptr);

    if (cell->player() == m_player && m_player == blue &&  !cell->isInverted()){ // ocorre na ida das peças azuis
        switch (col) { // o switch case para cerificar em qual coluna esta a peça que esta movimentando
            case 1: // anda 1 casa na ida
            case 5:
            n_row = row-1;
                while(m_board[n_row][col]->player() == red){ // verifica a existencia de pedras rivais no caminhi
                    if(m_board[n_row][col]->isInverted()){ // verifica se a peça no caminho é invertida
                        m_board[n_row][6]->setPlayer(red); // retorna a ponta que voltou
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red); // retorna a ponta inicial
                    }
                    m_board[n_row][col]->reset();// limpa a casa
                    n_row = n_row - 1;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);
                cell = m_board[n_row][col];
                if(n_row == 0){// se chegar ao fim inverte
                    cell->setInverted(true);
                }

                emit turnEnded();
                break;
            case 3: // anda duas casas
            n_row = row-1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row - 1;
                    i++;// flag para verificar se encontrou pedras azuis na primeira casa
                }
            if(i == 0 && n_row > 0 ){ // caso não tenha encontrado procura na segunda casa
                n_row = n_row - 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row - 1;
                    i++;
                }
            }
                if(n_row <= 0 ){
                    n_row = 0;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);
                cell = m_board[n_row][col];
                if(n_row == 0){
                    cell->setInverted(true);
                }

                emit turnEnded();
                break;
            case 2:
            case 4:
            n_row = row - 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row - 1;
                    i++;
                }
            if(i == 0 && n_row > 0){
                n_row = n_row - 1;
                while(m_board[n_row][col]->player() != nullptr){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row - 1;
                    i++;
                }
            }
            if(i == 0 && n_row > 0){
                n_row = n_row - 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row - 1;
                    i++;
                }
            }

                if(n_row <= 0){
                    n_row = 0;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);
                cell = m_board[n_row][col];
                if(n_row == 0){
                    cell->setInverted(true);

                }
                emit turnEnded();
                break;
        }
    }
    if (cell->player() == m_player && m_player == red && !cell->isInverted()){ //ocorre na ida das pedras vermelhas
        switch (row) {// funciona de maneira identica
            case 1:
            case 5:
            n_col = col+1;
                while(m_board[row][n_col]->player() == blue){

                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col + 1;
                }
                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                if(n_col == 6){
                    cell->setInverted(true);
                }

                emit turnEnded();
                break;
            case 3:
            n_col = col+1;

                while(m_board[row][n_col]->player() == blue){
                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col + 1;
                    i++;
                }

               if(i == 0 && n_col < 6){
                   n_col = n_col + 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col + 1;
                        i++;
                   }
                }
            if(n_col >= 6 ){
                n_col = 6;
            }
                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                if(n_col == 6){
                    cell->setInverted(true);
                }

                emit turnEnded();
                break;
            case 2:
            case 4:
            n_col = col+1;

                while(m_board[row][n_col]->player() == blue){
                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col + 1;
                    i++;
                }

               if(i == 0 && n_col < 6){
                   n_col = n_col + 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col + 1;
                        i++;
                   }
                }
               if(i == 0 && n_col < 6){
                   n_col = n_col + 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col + 1;
                        i++;
                   }
                }
               if(n_col >= 6 ){
                   n_col = 6;
               }

                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                if(n_col == 6){
                    cell->setInverted(true);
                }

                emit turnEnded();
                break;
        }
    }
    if (cell->player() == m_player && m_player == blue &&  cell->isInverted()){ // caso da volta pedras azuis
        switch (col) {// mudam os valores que andam 1 e 3 casas
            case 2:
            case 4:
            n_row = row+1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);

                cell = m_board[n_row][col];
                cell->setInverted(true);
                if(n_row == 6){
                    cell->QWidget::setEnabled(false);
                     m_player->incrementCount();
                     if(m_player->count() >= 4){
                         QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Azul Venceu.");
                         fim = 1;
                     }
                }
                if (fim == 0){
                    emit turnEnded();
                }else {
                    reset();
                }

                break;
            case 3:
            n_row = row+1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                    i++;
                }
            if(i == 0 && n_row < 6){
                n_row = n_row + 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                    i++;
                }
            }
                if(n_row >= 6){
                    n_row = 6;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);
                cell = m_board[n_row][col];
                cell->setInverted(true);
                if(n_row == 6){ // chega ao fim
                    m_player->incrementCount();
                    cell->QWidget::setEnabled(false); // desativa a pedra
                    if(m_player->count() >= 4){ // verifica se já venceu o azul
                        QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Azul Venceu.");
                        fim = 1;
                    }
               }
               if (fim == 0){
                   emit turnEnded();
               }else {
                   reset();
               }
                break;
            case 1:
            case 5:
            n_row = row+1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                    i++;
                }
            if(i == 0 && n_row < 6){
                n_row = n_row + 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                    i++;
                }
            }
            if(i == 0 && n_row < 6){
                n_row = n_row + 1;
                while(m_board[n_row][col]->player() == red){
                    if(m_board[n_row][col]->isInverted()){
                        m_board[n_row][6]->setPlayer(red);
                        m_board[n_row][6]->setInverted(true);
                    }else{
                        m_board[n_row][0]->setPlayer(red);
                    }
                    m_board[n_row][col]->reset();
                    n_row = n_row + 1;
                    i++;
                }
            }

                if(n_row >= 6){
                    n_row = 6;
                }
                cell->reset();
                m_board[n_row][col]->setPlayer(blue);
                cell = m_board[n_row][col];
                cell->setInverted(true);
                if(n_row == 6){
                   m_player->incrementCount();
                   cell->QWidget::setEnabled(false);
                   if(m_player->count() >= 4){
                       QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Azul Venceu.");
                       fim = 1;
                   }
              }
              if (fim == 0){
                  emit turnEnded();
              }else {
                  reset();
              }
                break;
        }
    }
    if (cell->player() == m_player && m_player == red && cell->isInverted()){ // ocorre na volta das edras vermelhas
        switch (row) {
            case 2:
            case 4:
            n_col = col-1;
                while(m_board[row][n_col]->player() == blue){

                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col - 1;

                }
                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                cell->setInverted(true);
                if(n_col == 0){
                    cell->QWidget::setEnabled(false);
                    m_player->incrementCount();
                    if(m_player->count() >= 4){
                        QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Vermelho Venceu.");
                        fim = 1;
                    }
               }
               if (fim == 0){
                   emit turnEnded();
               }else {
                   reset();
               }
                break;
            case 3:
            n_col = col-1;

                while(m_board[row][n_col]->player() == blue){
                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col - 1;
                    i++;
                }

               if(i == 0 && n_col > 0){
                   n_col = n_col - 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col - 1;
                        i++;
                   }
                }
            if(n_col <= 0 ){
                n_col = 0;
            }
                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                cell->setInverted(true);
                if(n_col == 0){
                    m_player->incrementCount();
                    cell->QWidget::setEnabled(false);
                    if(m_player->count() == 4){
                        QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Vermelho Venceu.");
                        fim = 1;
                    }
               }
               if (fim == 0){
                   emit turnEnded();
               }else {
                   reset();
               }
                break;
            case 1:
            case 5:
            n_col = col-1;

                while(m_board[row][n_col]->player() == blue){
                    if(m_board[row][n_col]->isInverted()){
                        m_board[0][n_col]->setPlayer(blue);
                        m_board[0][n_col]->setInverted(true);
                    }else{
                        m_board[6][n_col]->setPlayer(blue);
                    }
                    m_board[row][n_col]->reset();
                    n_col = n_col - 1;
                    i++;
                }

               if(i == 0 && n_col > 0){
                   n_col = n_col - 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col - 1;
                        i++;
                   }
                }
               if(i == 0 && n_col > 0){
                   n_col = n_col - 1;
                    while(m_board[row][n_col]->player() == blue){
                        if(m_board[row][n_col]->isInverted()){
                            m_board[0][n_col]->setPlayer(blue);
                            m_board[0][n_col]->setInverted(true);
                        }else{
                            m_board[6][n_col]->setPlayer(blue);
                        }
                        m_board[row][n_col]->reset();
                        n_col = n_col - 1;
                        i++;
                   }
                }
               if(n_col <= 0 ){
                   n_col = 0;
               }
                cell->reset();
                m_board[row][n_col]->setPlayer(red);
                cell = m_board[row][n_col];
                cell->setInverted(true);
                if(n_col == 0){
                    m_player->incrementCount();
                    cell->QWidget::setEnabled(false);
                    if(m_player->count() == 4){
                        QMessageBox::about(this,"Vencedor", "Parabéns!! O jogador Vermelho Venceu.");
                        fim = 1;
                    }
               }
               if (fim == 0){
                   emit turnEnded();
               }else {
                   reset();
               }
                break;
        }
    }
}



void Squadro::switchPlayer() {
    // Switch the player.
    m_player = m_player->other();

    // Finally, update the status bar.
    this->updateStatusBar();
}

void Squadro::reset() {
    // Reset the red player.
    Player* red = Player::player(Player::Red);
    red->reset();

    // Reset the blue player.
    Player* blue = Player::player(Player::Blue);
    blue->reset();

    // Reset board.
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 7; ++col) {
            Cell* cell = m_board[row][col];
            if (cell != nullptr)
                cell->reset();
        }
    }

    for (int i = 1; i < 6; i++) {
        m_board[i][0]->setPlayer(red);
        m_board[i][0]->QWidget::setEnabled(true);
        m_board[6][i]->setPlayer(blue);
        m_board[6][i]->QWidget::setEnabled(true);
    }

    // Set the starting player.
    m_player = red;

    // Finally, update the status bar.
    this->updateStatusBar();
}

void Squadro::showAbout() {
    QMessageBox::information(this, tr("About"),
        tr("Squadro\n\nAndrei Rimsa Alvares - andrei@cefetmg.br\nLucas Rafael Alves de Souza - lucasrafaelalvesd@gmail.com\nSergio H. Mendes de Assis - serginho.abreu@outlook.com"));
}

void Squadro::updateStatusBar() {
    ui->statusbar->showMessage(tr("Vez do %1 (%2 de 5)").arg(m_player->name()).arg(m_player->count()));
}
