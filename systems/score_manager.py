import json, os

class ScoreManager:
    GAMES = ['asteroid_shooting', 'whack_a_mole', 'space_maze', 'rocket_race']

    def __init__(self):
        self.path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'scores.json')
        self.scores = self._load()

    def _load(self):
        if os.path.exists(self.path):
            try:
                with open(self.path, 'r') as f:
                    return json.load(f)
            except Exception:
                pass
        return {g: [] for g in self.GAMES}

    def save(self):
        with open(self.path, 'w') as f:
            json.dump(self.scores, f, indent=2)

    def add_score(self, game, name, score):
        if game not in self.scores:
            self.scores[game] = []
        self.scores[game].append({'name': name, 'score': int(score)})
        self.scores[game] = sorted(self.scores[game], key=lambda x: x['score'], reverse=True)[:5]
        self.save()

    def get_top(self, game, n=5):
        return self.scores.get(game, [])[:n]

    def personal_best(self, game, name):
        scores = [s['score'] for s in self.scores.get(game, []) if s['name'] == name]
        return max(scores) if scores else 0
