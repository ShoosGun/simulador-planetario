#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


//Constantes (n�o t�o constantes) e outras var que precisam ser declaradas antes

const float pi{ static_cast<float>(std::acos(-1)) };
float constG{ 1.f };
float deltaTime{ 1.f / 60.f };


//Criar uma Class chamada DynamicBody
//Usar as var dos planetas


float Modulo(const sf::Vector2f &vector)
{
	return std::sqrt(vector.x*vector.x + vector.y*vector.y);
}
float Modulo(float x, float y)
{
	return std::sqrt(x*x + y*y);
}

sf::Vector2f Normalize(const sf::Vector2f &vector)
{
	float modulo{ Modulo(vector) };

	return sf::Vector2f(vector.x / modulo, vector.y / modulo);
}
sf::Vector2f Normalize( float x,  float y)
{
	float modulo{ Modulo(x,y) };

	return sf::Vector2f(x / modulo, y / modulo);
}






class DynamicBody
{
	sf::Vector2f m_aceleracao{ 0.f,0.f };
	sf::Vector2f m_velocidade{};

	float m_massa;

public:
	DynamicBody(float massa = 1.f): m_massa{massa}
	{
		
	}

	const sf::Vector2f getAceleracao() const { return m_aceleracao; }
	sf::Vector2f getAceleracao()  { return m_aceleracao; }

	
	void setAceleracao(sf::Vector2f aceleracao)
	{
		m_aceleracao.x = aceleracao.x;
		m_aceleracao.y = aceleracao.y;
	}
	void setAceleracao(float x, float y)
	{
		m_aceleracao.x = x;
		m_aceleracao.y = y;
	}
	

	const sf::Vector2f getVelocidade() const { return m_velocidade; }
	sf::Vector2f getVelocidade()  { return m_velocidade; }

	
	void setVelocidade(sf::Vector2f velocidade)
	{
		m_velocidade.x = velocidade.x;
		m_velocidade.y = velocidade.y;
	}
	void setVelocidade(float x, float y)
	{
		m_velocidade.x = x;
		m_velocidade.y = y;
	}

	const float getMassa() const { return m_massa; }
	float getMassa()  { return m_massa; }
	void setMassa(float massa)
	{
		m_massa = massa;
	}

	void MudancaDeVelocidade()
	{
		setVelocidade(getVelocidade() + (getAceleracao() * deltaTime));
	}

	
};

class Planeta:public DynamicBody
{

public:

	sf::CircleShape *  m_planeta{};
	
	
	Planeta(sf::CircleShape &formato, float massa = 1.f) :DynamicBody{ massa }, m_planeta{ &formato }
	{
		
	}

	sf::Vector2f ForcaDaGravidade(const Planeta &planeta)
	{
		
		sf::Vector2f raio{ planeta.m_planeta->getPosition() - m_planeta->getPosition() };
		float moduloDoRaio = Modulo(raio);

		//m*M*G/(r*r)
		return (planeta.getMassa()*getMassa()*constG / (moduloDoRaio*moduloDoRaio))*Normalize(raio);

	}

	void Deslocamento()
	{
		m_planeta->setPosition(m_planeta->getPosition() + (getVelocidade() * deltaTime));
	}
	

};


//Coisas com std::Vector
void CalcularForcasEntreTodos(const std::vector <Planeta *> &planetas)
{
	sf::Vector2f moduloDaForcaDaGravidade{ 0.f,0.f };

	int indexDoP{ 0 };
	int indexDoQ{ 0 };

	//Enquanto n�o pensar em algo melhor, vai ser assim
	for(Planeta * i : planetas)
	{
		i->setAceleracao(0.f,0.f );
	}

	for (Planeta * p : planetas)
	{
		for (Planeta * q : planetas)
		{

			if (indexDoQ > indexDoP) //se os planetas tiverem o index, aka serem iguais, ent�o n�o tem o porque calcular isso
			{			//E se eles ja foram(index menor) tbm n tem o porque (ja que funciona n�o importa a dire��o)
				moduloDaForcaDaGravidade = p->ForcaDaGravidade(*q);


				//Somar para o planeta 'q' e 'p' no final
				//Resetar depois o moduloDaForca... -> 0.f,0.f
				p->setAceleracao(p->getAceleracao() + (moduloDaForcaDaGravidade / p->getMassa()));
				q->setAceleracao( q->getAceleracao() -(moduloDaForcaDaGravidade / q->getMassa()));

				moduloDaForcaDaGravidade = sf::Vector2f(0.f, 0.f);
			}
			indexDoQ++;
		}

		indexDoP++;
		indexDoQ = 0;
	}

}


void MoverTodos(const std::vector <Planeta *> &planetas)
{
	for (Planeta * p : planetas)
	{

		p->MudancaDeVelocidade();
		p->Deslocamento();

	}
}

//window.draw(*planetA.m_planeta)
void DesenharTodos(const std::vector <Planeta *> &planetas,  sf::RenderWindow &window)
{
	for(Planeta * p : planetas)
	{
		window.draw(*p->m_planeta);
	}

}
//--


int main()
{
	//Lendo os arquivos

	std::ifstream arquivo{ "C:/Users/ivanr/source/repos/Graph/Debug/planetas.txt" };
	std::string linha{};
	std::string palavra{};
	std::vector <std::string> comando{};

	size_t posicao{0};


	std::vector <Planeta *> planetas{};
	unsigned int fps{ 60 };
	sf::Vector2f origem{ 500.f,250.f };

	

	if (arquivo.is_open())
	{
		while (std::getline(arquivo, linha)) // Estrutura: [comando] [o que �] [valor] ou [comando] [valor] ou [o que �] [valor] ou tudo igual mas ... [valor 1] [valor 2]
		{
			
			while ((posicao = linha.find(' ')) != std::string::npos)
			{
				palavra = linha.substr(0, posicao);
				
				linha.erase(0, posicao + 1);

				comando.push_back(palavra);
			}

			try 
			{
				//Comando de 2 parametros: [comando] [valor] ou [o que �] [valor] 
				//Eu sei, "mais que belo e otimizado c�digo  temos aki"
				if (comando.size() == 1) 
				{
					if (comando[0].compare("G") == 0)
					{
						constG = std::stof(linha);
					}

					if (comando[0].compare("fps") == 0)
					{
						fps = std::stoi(linha);
					}

					if (comando[0].compare("deltaTime") == 0)
					{
						deltaTime = 1/std::stof(linha);
					}

					if (comando[0].compare("Criar") == 0)
					{
						int numeroDePlanetas{ std::stoi(linha) };
						while (numeroDePlanetas > 0)
						{
							
							planetas.push_back(new Planeta(*new sf::CircleShape()));
							numeroDePlanetas--;
						}
					}

				}
				//[comando] [o que �] [valor]
				
				if (comando.size() == 2)
				{
					
					if (comando[0].compare("Massa") == 0)
					{
						if (planetas.size() >= std::stoi(comando[1]))
							planetas[std::stoi(comando[1])]->setMassa(std::stof(linha));

					}

					if (comando[0].compare("Tamanho") == 0)
					{
						if (planetas.size() >= std::stoi(comando[1])) 
						{
							planetas[std::stoi(comando[1])]->m_planeta->setRadius(std::stof(linha)); 

							planetas[std::stoi(comando[1])]->m_planeta->setOrigin(std::stof(linha), std::stof(linha));
						}
					}


				}

				//[comando] [o que �] [valor 1] [valor 2]

				if(comando.size() == 3)
				{
					if (comando[0].compare("Posicao") == 0)
					{
						if (planetas.size() >= std::stoi(comando[1]))
							planetas[std::stoi(comando[1])]->m_planeta->setPosition(sf::Vector2f(std::stof(comando[2]), std::stof(linha)) + origem);


					}

					if (comando[0].compare("Velocidade") == 0)
					{
						if (planetas.size() >= std::stoi(comando[1]))
							planetas[std::stoi(comando[1])]->setVelocidade(std::stof(comando[2]), std::stof(linha));


					}
				}

				//[comando] [o que �] [valor 1] [valor 2] [valor 3]
				if (comando.size() == 4)
				{
					if (comando[0].compare("Cor") == 0)
					{
						if (planetas.size() >= std::stoi(comando[1]))
							planetas[std::stoi(comando[1])]->m_planeta->setFillColor(sf::Color(std::stoi(comando[2]),std::stoi(comando[3]), std::stoi(linha)) );


					}
				}
			}
			catch(std::exception e)
			{
				std::cout << "Deu erro" << e.what();
			}

			

			comando.clear();
		}

		arquivo.close();
	}
	else
		std::cout << "Nao consegui abrir o arquivo";

	std::cout << constG;
	

	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	sf::RenderWindow window(sf::VideoMode(1000,500),"Orbits",sf::Style::Default, settings);

	window.setFramerateLimit(fps);

	

	sf::View view(origem, sf::Vector2f(1000.f, 500.f));

	
	

	/*DynamicBody navinha{};

	sf::VertexArray navinhaForma(sf::Quads,4);

	navinhaForma[0].position = sf::Vector2f(0.f, 0.f ) ;
	navinhaForma[0].color = sf::Color::Blue;
	navinhaForma[1].position = sf::Vector2f(100.f, 0.f) ;
	navinhaForma[1].color = sf::Color::Green;
	navinhaForma[2].position = sf::Vector2f(100.f, 100.f) ;
	navinhaForma[2].color = sf::Color::White;
	navinhaForma[3].position = sf::Vector2f(0.f, 100.f) ;
	navinhaForma[3].color = sf::Color::Red;

	

	sf::Vector2f navinhaCentro{50.f,50.f};
	sf::Vector2f navinhaPosicao{ origem };*/
	

	
	view.zoom(0.01f);
	
	window.setView(view);


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		


		window.clear();
		
		CalcularForcasEntreTodos(planetas);
		
		MoverTodos(planetas);
			   		 
		DesenharTodos(planetas, window);


		/*navinha.setAceleracao(0.f, 0.f);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2i localPosition = sf::Mouse::getPosition(window);
			navinha.setAceleracao(5.f * Normalize( localPosition.x - navinhaPosicao.x, localPosition.y - navinhaPosicao.y));
			
		}
		navinha.MudancaDeVelocidade();
		navinhaPosicao = navinha.getVelocidade()*deltaTime + navinhaPosicao;*/

		//COLIS�ES entre retangulos e circulos: (quadrado, circulo)

		//C:\Users\ivanr\Desktop\algoritimo bola quadrado.png (o algoritimo)








		//Ajeitar os vertex da navinha
		//Criar classe com todos esse atributos e chamar BetterQuads
		/*sf::VertexArray vertexForDrawing(sf::Quads, navinhaForma.getVertexCount());
		for (int i = 0; i<navinhaForma.getVertexCount();i++)
		{
			vertexForDrawing[i].position = (navinhaForma[i].position - navinhaCentro) + navinhaPosicao;
			vertexForDrawing[i].color = navinhaForma[i].color;
		}

		window.draw(vertexForDrawing);*/
		

		window.display();
	
	}

}