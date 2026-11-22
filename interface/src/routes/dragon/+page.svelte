<script lang="ts">
	import { onDestroy, onMount } from 'svelte';
	import { socket } from '$lib/stores/socket';
	import { notifications } from '$lib/components/toasts/notifications';

	type MotorState = {
		position_mm: number;
		stroke_mm: number;
		position_ratio: number;
		moving: boolean;
		position_known: boolean;
	};

	type DragonState = {
		motors: Record<string, MotorState>;
		leds: Record<string, 'off' | 'on' | 'timed'>;
	};

	let dragonState = $state<DragonState>({ motors: {}, leds: {} });
	let draftPositions = $state<Record<string, number>>({});
	let customTarget = $state('graoully');
	let customAction = $state('status');
	let selectedShow = $state('SLEEP');
	let lastUpdate = $state('');
	let commandStatus = $state('En attente de l’état du dragon…');

	const motorLabels: Record<string, string> = {
		wings_top: 'Ouverture des ailes',
		wings_bot: 'Battement des ailes',
		pan: 'Rotation de la tête',
		tilt: 'Inclinaison de la tête',
		roll: 'Rotation latérale',
		eyelids: 'Paupières',
		ears: 'Oreilles',
		mouth: 'Mâchoire'
	};

	const receiveState = (state: DragonState) => {
		dragonState = state;
		for (const [name, motor] of Object.entries(state.motors)) {
			if (!(name in draftPositions)) draftPositions[name] = motor.position_ratio;
		}
		lastUpdate = new Date().toLocaleTimeString();
	};

	onMount(() => socket.on<DragonState>('dragonState', receiveState));
	onDestroy(() => socket.off('dragonState', receiveState));

	function sendCommand(target: string, action: string) {
		socket.sendEvent('dragonCommand', { target, action });
		commandStatus = `Commande envoyée : ${target}:${action}`;
	}

	function sendMotorPosition(name: string) {
		const ratio = Math.max(0, Math.min(1, draftPositions[name] ?? 0));
		sendCommand(name, ratio.toFixed(2));
	}

	function toggleLed(name: string) {
		sendCommand(name, dragonState.leds[name] === 'on' ? 'off' : 'on');
	}

	function sendCustomCommand(event: SubmitEvent) {
		event.preventDefault();
		if (!customTarget.trim() || !customAction.trim()) {
			notifications.warning('Indique une cible et une action.', 3000);
			return;
		}
		sendCommand(customTarget.trim(), customAction.trim());
	}
</script>

<svelte:head>
	<title>Dragon Control</title>
</svelte:head>

<div class="mx-0 my-1 flex flex-col gap-4 sm:mx-8 sm:my-8">
	<section class="card bg-base-100 shadow-xl">
		<div class="card-body">
			<div class="flex flex-wrap items-center justify-between gap-3">
				<div>
					<h1 class="card-title text-2xl">Contrôle du dragon</h1>
					<p class="text-sm opacity-70">Simulation ESP32 — état actualisé en direct{lastUpdate ? ` · ${lastUpdate}` : ''}</p>
				</div>
				<div class="flex flex-wrap gap-2">
					<button class="btn btn-warning" onclick={() => sendCommand('graoully', 'home')}>Home</button>
					<button class="btn btn-error" onclick={() => sendCommand('graoully', 'stop')}>Stop</button>
					<button class="btn btn-secondary" onclick={() => sendCommand('graoully', 'reset')}>Reset</button>
				</div>
			</div>
			<p class="text-sm" aria-live="polite">{commandStatus}</p>
			<div class="alert alert-info">
				<span>Les positions affichées sont estimées par le modèle de vitesse/course. Sans capteurs, elles ne mesurent pas la position physique.</span>
			</div>
		</div>
	</section>

	<section class="grid gap-4 md:grid-cols-2">
		{#each Object.entries(dragonState.motors) as [name, motor] (name)}
			<div class="card bg-base-100 shadow-lg">
				<div class="card-body gap-3">
					<div class="flex items-center justify-between gap-2">
						<h2 class="card-title">{motorLabels[name] ?? name}</h2>
						<span class:badge-success={!motor.moving} class:badge-warning={motor.moving} class="badge">
							{motor.moving ? 'En mouvement' : 'À l’arrêt'}
						</span>
					</div>
					<div class="flex justify-between text-sm">
						<span>{motor.position_known ? `${motor.position_mm.toFixed(1)} / ${motor.stroke_mm.toFixed(0)} mm` : 'Position inconnue'}</span>
						<span>{motor.position_known ? `${(motor.position_ratio * 100).toFixed(0)} %` : '—'}</span>
					</div>
					<progress class="progress progress-primary w-full" value={Math.max(0, motor.position_ratio * 100)} max="100"></progress>
					<div class="join w-full">
						<input
							class="range range-primary join-item"
							type="range"
							min="0"
							max="1"
							step="0.01"
							value={draftPositions[name] ?? motor.position_ratio}
							oninput={(event) => (draftPositions[name] = Number(event.currentTarget.value))}
							aria-label={`Position ${motorLabels[name] ?? name}`}
						/>
						<button class="btn btn-primary join-item" onclick={() => sendMotorPosition(name)}>Déplacer</button>
					</div>
				</div>
			</div>
		{/each}
	</section>

	<section class="card bg-base-100 shadow-xl">
		<div class="card-body">
			<h2 class="card-title">Éléments lumineux</h2>
			<div class="flex flex-wrap gap-2">
				{#each Object.entries(dragonState.leds) as [name, state] (name)}
					<button class="btn" class:btn-success={state === 'on'} onclick={() => toggleLed(name)}>
						{name} : {state} · cliquer pour basculer
					</button>
				{/each}
			</div>
		</div>
	</section>

	<section class="card bg-base-100 shadow-xl">
		<div class="card-body">
			<h2 class="card-title">Shows</h2>
			<div class="flex flex-wrap gap-2">
				<select class="select select-bordered" bind:value={selectedShow} aria-label="Préréglage du show">
					<option value="SLEEP">SLEEP</option>
					<option value="ALERT">ALERT</option>
					<option value="WAKE">WAKE</option>
				</select>
				<button class="btn btn-primary" onclick={() => sendCommand('show', `play?${selectedShow}`)}>Lancer</button>
				<button class="btn btn-error" onclick={() => sendCommand('show', 'stop')}>Arrêter le show</button>
			</div>
		</div>
	</section>

	<section class="card bg-base-100 shadow-xl">
		<div class="card-body">
			<h2 class="card-title">Commande manuelle</h2>
			<form class="flex flex-wrap gap-2" onsubmit={sendCustomCommand}>
				<input class="input input-bordered" bind:value={customTarget} placeholder="Cible (ex. pan, eyes, graoully)" aria-label="Cible" />
				<input class="input input-bordered" bind:value={customAction} placeholder="Action (ex. 0.5, on, home)" aria-label="Action" />
				<button class="btn btn-accent" type="submit">Envoyer</button>
			</form>
			<p class="text-sm opacity-70">Format repris des commandes série : cible:action. Les commandes de séquence CSV restent à saisir depuis le port série.</p>
		</div>
	</section>
</div>