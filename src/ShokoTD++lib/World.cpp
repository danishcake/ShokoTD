#include "World.h"
#include <assert.h>
#include <TinyXML.h>
#include <algorithm>
#include "Walker.h"
#include <functional>

namespace SquareType
{
	Direction::Enum GetDirection(Enum _square_type)
	{
		switch(_square_type)
		{
		case NorthArrow:
			return Direction::North;
		case SouthArrow:
			return Direction::South;
		case WestArrow:
			return Direction::West;
		case EastArrow:
			return Direction::East;
		case HalfNorthArrow:
			return Direction::North;
		case HalfSouthArrow:
			return Direction::South;
		case HalfWestArrow:
			return Direction::West;
		case HalfEastArrow:
			return Direction::East;
		default:
			return Direction::Stopped;
		}
	}

	Enum ArrowFromDirection(Direction::Enum _direction)
	{
		switch(_direction)
		{
		case Direction::North:
			return NorthArrow;
		case Direction::South:
			return SouthArrow;
		case Direction::West:
			return WestArrow;
		case Direction::East:
			return EastArrow;
		default:
			return Empty;
		}
	}

	Enum SpawnerFromDirection(Direction::Enum _direction)
	{
		switch(_direction)
		{
		case Direction::North:
			return NorthSpawner;
		case Direction::South:
			return SouthSpawner;
		case Direction::West:
			return WestSpawner;
		case Direction::East:
			return EastSpawner;
		default:
			return Empty;
		}
	}

	Enum Diminish(Enum _square_type)
	{
		switch(_square_type)
		{
		case NorthArrow:
			return SquareType::HalfNorthArrow;
		case SouthArrow:
			return SquareType::HalfSouthArrow;
		case WestArrow:
			return SquareType::HalfWestArrow;
		case EastArrow:
			return SquareType::HalfEastArrow;
		case HalfNorthArrow:
		case HalfSouthArrow:
		case HalfWestArrow:		
		case HalfEastArrow:
			return SquareType::Empty;
		default:
			return SquareType::Empty;
		}
	}
	Enum RestoreToFull(Enum _square_type)
	{
		switch(_square_type)
		{
		case HalfNorthArrow:
			return NorthArrow;
		case HalfSouthArrow:
			return SouthArrow;
		case HalfWestArrow:
			return WestArrow;
		case HalfEastArrow:
			return EastArrow;
		default:
			return _square_type;
		}
	}
}

World::World(void)
{
	sum_time_ = 0;
	size_ = Vector2i(20, 20);
	walls_ = vector<vector<TopLeft>>(size_.x, vector<TopLeft>(size_.y));
	special_squares_ = vector<vector<SquareType::Enum>>(size_.x, vector<SquareType::Enum>(size_.y));
	for(int x = 0; x < size_.x; x++)
		walls_[x][0].top = true;
	for(int y = 0; y < size_.y; y++)
		walls_[0][y].left = true;
	name_ = "Default 20x20";
	state_ = WorldState::OK;
}

World::World(std::string _filename)
{
	sum_time_ = 0;
	filename_ = _filename;
	_filename = "Levels/" + _filename; 
	state_ = WorldState::OK;
	TiXmlDocument document = TiXmlDocument(_filename.c_str());
	TiXmlHandle document_handle = TiXmlHandle(&document);
	if(document.LoadFile())
	{
		TiXmlElement* pLevel = document_handle.FirstChild("Level").Element();
		TiXmlElement* pName = document_handle.FirstChild("Level").FirstChild("Name").Element();
		TiXmlElement* pSize = document_handle.FirstChild("Level").FirstChild("Size").Element();
		if(pLevel)
		{
			if(pName)
				name_ = pName->GetText();
			else
				name_ = "Unnamed level";

			if(pSize)
			{
				if(pSize->QueryIntAttribute("x", &size_.x) != TIXML_SUCCESS)
					state_ = WorldState::FileLoadError;
				if(pSize->QueryIntAttribute("y", &size_.y) != TIXML_SUCCESS)
					state_ = WorldState::FileLoadError;
				if(state_ == WorldState::OK)
				{
					walls_ = vector<vector<TopLeft>>(size_.x, vector<TopLeft>(size_.y));
					special_squares_ = vector<vector<SquareType::Enum>>(size_.x, vector<SquareType::Enum>(size_.y));
				}
			} else
				state_ = WorldState::FileLoadError;
			//Read list of 'H' elements representing horizontal walls
			TiXmlElement* h = document_handle.FirstChild("Level").FirstChild("H").Element();
			while(h)
			{
				Vector2i point;
				bool attribute_error = false;
				attribute_error |= (h->QueryIntAttribute("x", &point.x) != TIXML_SUCCESS);
				attribute_error |= (h->QueryIntAttribute("y", &point.y) != TIXML_SUCCESS);
				if(!attribute_error)
				{ 
					walls_[point.x][point.y].top = true;
				} else
				{}//TODO error 
				h = h->NextSiblingElement("H");
			}

			//Read list of 'V' elements representing vertical walls
			TiXmlElement* v = document_handle.FirstChild("Level").FirstChild("V").Element();
			while(v)
			{
				Vector2i point;
				bool attribute_error = false;
				attribute_error |= (v->QueryIntAttribute("x", &point.x) != TIXML_SUCCESS);
				attribute_error |= (v->QueryIntAttribute("y", &point.y) != TIXML_SUCCESS);
				if(!attribute_error)
				{ 
					walls_[point.x][point.y].left = true;
				} else
				{}//TODO error 
				v = v->NextSiblingElement("V");
			}
			//Load all rockets
			TiXmlElement* rocket = document_handle.FirstChild("Level").FirstChild("Rocket").Element();
			while(rocket)
			{
				Vector2i point;
				bool attribute_error = false;
				attribute_error |= (rocket->QueryIntAttribute("x", &point.x) != TIXML_SUCCESS);
				attribute_error |= (rocket->QueryIntAttribute("y", &point.y) != TIXML_SUCCESS);
				
				if(!attribute_error)
				{ 
					SetSquareType(point, SquareType::Rocket);
					
				} else
				{}//TODO error 
				rocket = rocket->NextSiblingElement("Rocket");
			}

			//Load all holes
			TiXmlElement* hole = document_handle.FirstChild("Level").FirstChild("Hole").Element();
			while(hole)
			{
				Vector2i point;
				bool attribute_error = false;
				attribute_error |= (hole->QueryIntAttribute("x", &point.x) != TIXML_SUCCESS);
				attribute_error |= (hole->QueryIntAttribute("y", &point.y) != TIXML_SUCCESS);
				
				if(!attribute_error)
				{ 
					SetSquareType(point, SquareType::Hole);
					
				} else
				{}//TODO error 
				hole = hole->NextSiblingElement("Hole");
			}

			TiXmlElement* wave = document_handle.FirstChild("Level").FirstChild("Wave").Element();
			while(wave)
			{
				float sum_weights = 0;
				int enemy_count = 0;
				float spawn_gap = 0;
				float start_time = 0;
				bool attribute_error = false;

				attribute_error |= (wave->QueryValueAttribute("number", &enemy_count) != TIXML_SUCCESS);
				attribute_error |= (wave->QueryValueAttribute("gap", &spawn_gap) != TIXML_SUCCESS);
				attribute_error |= (wave->QueryValueAttribute("start", &start_time) != TIXML_SUCCESS);

				std::vector<WaveComponent> components;
				TiXmlElement* enemy_type = wave->FirstChildElement("EnemyType");
				while(enemy_type)
				{
					WaveComponent wc;
					wc.weight = 1;
					enemy_type->QueryValueAttribute("weight", &wc.weight);
					if(enemy_type->QueryValueAttribute("type", &wc.enemy_type) != TIXML_SUCCESS)
					{
						attribute_error = true;
						//TODO error
					}
					components.push_back(wc);

					sum_weights += wc.weight;
					enemy_type = enemy_type->NextSiblingElement("EnemyType");
				}

				for(std::vector<WaveComponent>::iterator it = components.begin(); it != components.end(); ++it)
				{
					it->weight /= sum_weights;
				}
				if(!attribute_error)
				{
					Wave w;
					w.enemy_count = enemy_count;
					w.spawn_time_gap = spawn_gap;
					w.start_time = start_time;
					w.enemy_types = components;
					active_waves_.push_back(w);
				}


				wave = wave->NextSiblingElement("Wave");
			}
			TiXmlElement* spawner = document_handle.FirstChild("Level").FirstChild("Spawner").Element();
			while(spawner)
			{
				bool attribute_error = false;
				int x = 0;
				int y = 0;
				std::string d = "West";
				attribute_error |= (spawner->QueryValueAttribute("x", &x) != TIXML_SUCCESS);
				attribute_error |= (spawner->QueryValueAttribute("y", &y) != TIXML_SUCCESS);
				attribute_error |= (spawner->QueryValueAttribute("d", &d) != TIXML_SUCCESS);
				
				if(!attribute_error)
				{
					Spawner sp;
					sp.Position = Vector2i(x, y);
					sp.Direction = Direction::FromString(d);
					SetSquareType(sp.Position, SquareType::SpawnerFromDirection(sp.Direction));
					spawners_.push_back(sp);
				}

				spawner = spawner->NextSiblingElement("Spawner");
			}

		} else
		{
			state_ = WorldState::FileLoadError;
			//TODO error
		}
	} else
	{
		state_ = WorldState::FileLoadError;
		size_ = Vector2i(20, 20);
		walls_ = vector<vector<TopLeft>>(size_.x, vector<TopLeft>(size_.y));
		special_squares_ = vector<vector<SquareType::Enum>>(size_.x, vector<SquareType::Enum>(size_.y));
		name_ = "Unnamed level - load failed";
		//TODO Log error not worked, load default
	}
}

void World::SetSize(Vector2i _size)
{
	size_ = _size;
	walls_ = vector<vector<TopLeft>>(size_.x, vector<TopLeft>(size_.y));
	for(int x = 0; x < size_.x; x++)
		walls_[x][0].top = true;
	for(int y = 0; y < size_.y; y++)
		walls_[0][y].left = true;
	special_squares_ = vector<vector<SquareType::Enum>>(size_.x, vector<SquareType::Enum>(size_.y));
}

World::~World(void)
{
	Reset(); //Moves dead enemies to live list
	for(vector<Walker*>::iterator it = enemies_.begin(); it != enemies_.end(); ++it)
	{
		 delete *it;
	}
}

GridSquare World::GetGridSquare(Vector2i _point)
{
	if(!(_point.x >= 0 && _point.x < size_.x && _point.y >= 0	&& _point.y < size_.y))
		assert(false);
	GridSquare gs;
	gs.SetNorth(walls_[_point.x][_point.y].top);
	gs.SetWest(walls_[_point.x][_point.y].left);
	//Ugly ternery operators are wrap arounds 
	gs.SetSouth(walls_[_point.x][_point.y == size_.y - 1 ? 0 : _point.y + 1].top);
	gs.SetEast(walls_[_point.x == size_.x - 1 ? 0 : _point.x + 1][_point.y].left);

	return gs;
}

void World::SetGridSquare(Vector2i _point, GridSquare _gs)
{
	assert(_point.x >= 0 && _point.x < size_.x && _point.y >= 0	&& _point.y < size_.y);
	walls_[_point.x][_point.y].top = _gs.GetNorth();
	walls_[_point.x][_point.y].left = _gs.GetWest();
	//Ugly ternery operators are wrap arounds 
	walls_[_point.x][_point.y == size_.y - 1 ? 0 : _point.y + 1].top = _gs.GetSouth();
	walls_[_point.x == size_.x - 1 ? 0 : _point.x + 1][_point.y].left = _gs.GetEast();
}

void World::ToggleWall(Vector2i _position, Direction::Enum _direction)
{
	assert(_position.x >= 0 && _position.x < size_.x && _position.y >= 0	&& _position.y < size_.y);
	GridSquare gs = GetGridSquare(_position);
	gs.ToggleDirection(_direction);
	SetGridSquare(_position, gs);
}

SquareType::Enum World::GetSquareType(Vector2i _point)
{
	assert(_point.x >= 0 && _point.x < size_.x && _point.y >= 0	&& _point.y < size_.y);
	return special_squares_[_point.x][_point.y];
}

bool World::SetSquareType(Vector2i _point, SquareType::Enum _square_type)
{
	assert(_point.x >= 0 && _point.x < size_.x && _point.y >= 0	&& _point.y < size_.y);
	if(SquareType::GetDirection(_square_type) != Direction::Stopped)
	{
		if(special_squares_[_point.x][_point.y] == SquareType::Empty)
		{
			special_squares_[_point.x][_point.y] = _square_type;
			//TODO diminish stock
			return true;
		} else if(SquareType::GetDirection(special_squares_[_point.x][_point.y]) != Direction::Stopped)
		{
			//TODO free and then diminish stock
			special_squares_[_point.x][_point.y] = _square_type;
			return true;
		}
		return false;
	} else
	{
		special_squares_[_point.x][_point.y] = _square_type;
		return true;
	}
}

void World::ToggleRocket(Vector2i _position)
{
	assert(_position.x >= 0 && _position.x < size_.x && _position.y >= 0	&& _position.y < size_.y);
	if(special_squares_[_position.x][_position.y] == SquareType::Rocket)
		special_squares_[_position.x][_position.y] = SquareType::Empty;
	else
		special_squares_[_position.x][_position.y] = SquareType::Rocket;
}

void World::ToggleHole(Vector2i _position)
{
	assert(_position.x >= 0 && _position.x < size_.x && _position.y >= 0	&& _position.y < size_.y);
	if(special_squares_[_position.x][_position.y] == SquareType::Hole)
		special_squares_[_position.x][_position.y] = SquareType::Empty;
	else
		special_squares_[_position.x][_position.y] = SquareType::Hole;
}



WorldState::Enum World::Tick(float _dt)
{
	sum_time_ += _dt;
	//TODO perhaps split large DT into smaller periods?

	if(state_ == WorldState::OK)
	{
		//Tick waves
		for(vector<Wave>::iterator it = active_waves_.begin(); it != active_waves_.end(); ++it)
		{
			if(sum_time_ > it->start_time)
			{
				//Spawn enemy
				it->enemy_count--;
				it->start_time = sum_time_ + it->spawn_time_gap;
				for(vector<Spawner>::iterator it2 = spawners_.begin(); it2 != spawners_.end(); ++it2)
				{
					Walker* spawned_walker = it->Spawn();
					spawned_walker->SetPosition(it2->Position);
					spawned_walker->SetDirection(it2->Direction);
					spawned_walker->SetWorld(this);
					enemies_.push_back(spawned_walker);
				}
			}
			if(it->enemy_count == 0)
				finsished_waves_.push_back(*it);
		}
		active_waves_.erase(std::remove_if(active_waves_.begin(),active_waves_.end(),
							&Wave::IsDone), active_waves_.end());

		//Walk forward
		for(vector<Walker*>::iterator it = enemies_.begin(); it != enemies_.end(); ++it)
		{
			(*it)->Advance(_dt);
		}

		//Remove enemies that have just died, from holes, rockets and towers etc
		for(vector<Walker*>::iterator it = just_dead_enemies_.begin(); it != just_dead_enemies_.end(); ++it)
		{
			enemies_.erase(std::remove(enemies_.begin(), enemies_.end(), *it), enemies_.end());
			dead_enemies_.push_back(*it);
		}
		just_dead_enemies_.clear();

		if(state_ == WorldState::OK && active_waves_.size() == 0 && enemies_.size() == 0)
		{
			state_ = WorldState::Victory;
		}

		
	}

	//Tick enemies
	for(vector<Walker*>::iterator it = dead_enemies_.begin(); it != dead_enemies_.end(); ++it)
	{
		(*it)->DeathTick(_dt);
	}

	return state_;
}

void World::SaveAs(string _filename)
{
	Reset();
	_filename = "Levels/" + _filename; 
	TiXmlDocument document = TiXmlDocument(_filename);
	TiXmlHandle document_handle = TiXmlHandle(&document);
	TiXmlNode* pDeclaration = new TiXmlDeclaration();
	document.LinkEndChild(pDeclaration);
	TiXmlElement* pLevel = new TiXmlElement("Level");
	document.LinkEndChild(pLevel);
	TiXmlElement* pName = new TiXmlElement("Name");
	pLevel->LinkEndChild(pName);
	pName->LinkEndChild(new TiXmlText(name_));
	TiXmlElement* pSize = new TiXmlElement("Size");
	pSize->SetAttribute("x", size_.x);
	pSize->SetAttribute("y", size_.y);
	pLevel->LinkEndChild(pSize);
	

	for(int x = 0; x < size_.x; x++)
	{
		for(int y = 0; y < size_.y; y++)
		{
			if(walls_[x][y].left)
			{
				TiXmlElement* pLeftWall = new TiXmlElement("V");
				pLeftWall->SetAttribute("x", x);
				pLeftWall->SetAttribute("y", y);
				pLevel->LinkEndChild(pLeftWall);
			}
		}
	}
	for(int x = 0; x < size_.x; x++)
	{
		for(int y = 0; y < size_.y; y++)
		{
			if(walls_[x][y].top)
			{
				TiXmlElement* pTopWall = new TiXmlElement("H");
				pTopWall->SetAttribute("x", x);
				pTopWall->SetAttribute("y", y);
				pLevel->LinkEndChild(pTopWall);
			}
		}
	}
	for(int x = 0; x < size_.x; x++)
	{
		for(int y = 0; y < size_.y; y++)
		{
			if(special_squares_[x][y] == SquareType::Hole)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Hole");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pLevel->LinkEndChild(pSpecialSquare);
			} else if(special_squares_[x][y] == SquareType::Rocket)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Rocket");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pLevel->LinkEndChild(pSpecialSquare);
			} else if (special_squares_[x][y] == SquareType::WestArrow)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Arrow");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pSpecialSquare->SetAttribute("d", "West");
				pLevel->LinkEndChild(pSpecialSquare);
			} else if (special_squares_[x][y] == SquareType::EastArrow)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Arrow");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pSpecialSquare->SetAttribute("d", "East");
				pLevel->LinkEndChild(pSpecialSquare);
			} else if (special_squares_[x][y] == SquareType::NorthArrow)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Arrow");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pSpecialSquare->SetAttribute("d", "North");
				pLevel->LinkEndChild(pSpecialSquare);
			} else if (special_squares_[x][y] == SquareType::SouthArrow)
			{
				TiXmlElement* pSpecialSquare = new TiXmlElement("Arrow");
				pSpecialSquare->SetAttribute("x", x);
				pSpecialSquare->SetAttribute("y", y);
				pSpecialSquare->SetAttribute("d", "South");
				pLevel->LinkEndChild(pSpecialSquare);
			}

		}
	}
	document.SaveFile();
}

void World::ClearArrows()
{
	for(int x = 0; x < size_.x; x++)
	{
		for(int y= 0; y < size_.y; y++)
		{
			if(SquareType::GetDirection(special_squares_[x][y]) != Direction::Stopped)
			{
				special_squares_[x][y] = SquareType::Empty;
			}
		}
	}
}

void World::Reset()
{
	just_dead_enemies_.clear();

	for(int x = 0; x < size_.x; x++)
	{
		for(int y= 0; y < size_.y; y++)
		{
			//TODO clear everything save
			special_squares_[x][y] = SquareType::RestoreToFull(special_squares_[x][y]);
		}
	}
	
	problem_points_.clear();

	if(state_ !=  WorldState::FileLoadError)
		state_ = WorldState::OK;
}

void World::WalkerReachNewSquare(Walker* _walker)
{
	//This method will be called multiple times, so it's important not to add an item to 'just_dead...' twice
	Vector2i position_grid = Vector2i(floor(_walker->GetPosition().x + 0.5f), floor(_walker->GetPosition().y + 0.5f));
	SquareType::Enum square_type = special_squares_[position_grid.x][position_grid.y];
	if(square_type == SquareType::Hole)
	{
		if(std::find(just_dead_enemies_.begin(), just_dead_enemies_.end(), _walker) == just_dead_enemies_.end())
		{
			just_dead_enemies_.push_back(_walker);
			problem_points_.push_back(_walker->GetPosition());
			_walker->Kill();
			//TODO increase score
		}
	} else if(square_type == SquareType::Rocket)
	{
		if(std::find(just_dead_enemies_.begin(), just_dead_enemies_.end(), _walker) == just_dead_enemies_.end())
		{
			just_dead_enemies_.push_back(_walker);
			_walker->Kill();
			//TODO decrease lives
		}
	}
	
	
	if(SquareType::GetDirection(square_type) != Direction::Stopped)
	{
		Direction::Enum direction = SquareType::GetDirection(square_type);
		if(SquareType::GetDirection(square_type) == Direction::TurnAround(_walker->GetDirection()))
		{
			special_squares_[position_grid.x][position_grid.y] = SquareType::Diminish(square_type);
		}
		_walker->EncounterArrow(direction);
	}
}

void World::ToggleArrow(Vector2i _position, Direction::Enum _direction)
{
	assert(_direction != Direction::Stopped);
	//TODO bounds checking assertions
	if(SquareType::GetDirection(special_squares_[_position.x][_position.y]) == _direction)
	{
		special_squares_[_position.x][_position.y] = SquareType::Empty;
	} else if(special_squares_[_position.x][_position.y] == SquareType::Empty)
	{
		special_squares_[_position.x][_position.y] = SquareType::ArrowFromDirection(_direction);
	} else if(SquareType::GetDirection(special_squares_[_position.x][_position.y]) != Direction::Stopped)
	{
		special_squares_[_position.x][_position.y] = SquareType::ArrowFromDirection(_direction);
	}
}

void World::ClearArrow(Vector2i _position)
{
	Direction::Enum direction = SquareType::GetDirection(special_squares_[_position.x][_position.y]);
	if(direction != Direction::Stopped)
	{
		special_squares_[_position.x][_position.y] = SquareType::Empty;
	}
}

Walker* Wave::Spawn()
{
	assert(enemy_types.size() > 0);
	double pick_frac = (double)rand() / (double)RAND_MAX;
	int index = -1;
	while(pick_frac > 0)
	{
		index++;
		pick_frac -= enemy_types[index].weight;
	}
	std::string picked_enemy = enemy_types[index].enemy_type;
//	std::string 
	Walker* wkr = new Walker();
	return wkr;
}